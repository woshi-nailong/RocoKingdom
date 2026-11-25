$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Drawing
Add-Type -AssemblyName Microsoft.VisualBasic

Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;

public class Win32 {
    [StructLayout(LayoutKind.Sequential)]
    public struct RECT { public int Left; public int Top; public int Right; public int Bottom; }

    [StructLayout(LayoutKind.Sequential)]
    public struct POINT { public int X; public int Y; }

    public delegate bool EnumProc(IntPtr hWnd, IntPtr lParam);

    [DllImport("user32.dll")] public static extern bool SetProcessDPIAware();
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr lParam);
    [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr hWnd);
    [DllImport("user32.dll")] public static extern bool IsWindow(IntPtr hWnd);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint pid);
    [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr hWnd, out RECT r);
    [DllImport("user32.dll")] public static extern bool ClientToScreen(IntPtr hWnd, ref POINT p);
    [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr hWnd);
    [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr hWnd, int cmd);
    [DllImport("user32.dll")] public static extern bool SetWindowPos(IntPtr hWnd, IntPtr after, int x, int y, int cx, int cy, uint flags);
    [DllImport("user32.dll")] public static extern bool SetCursorPos(int x, int y);
    [DllImport("user32.dll")] public static extern void mouse_event(uint flags, uint dx, uint dy, uint data, UIntPtr extra);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr hWnd, StringBuilder sb, int n);
}
"@

# make this process DPI aware so window coords and screen capture use the same physical pixel space
[void][Win32]::SetProcessDPIAware()

# project root = <root>\docs\capture.ps1
$root = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
$exe = Join-Path $root 'RocoKingdom\x64\Debug\RocoKingdom.exe'
$workdir = Join-Path $root 'RocoKingdom\RocoKingdom'
$outDir = Join-Path $root 'docs\screenshots'
New-Item -ItemType Directory -Force -Path $outDir | Out-Null

function Find-GameWindow([int]$procId) {
    $script:found = [IntPtr]::Zero
    $script:foundRank = 0
    $script:bestArea = 0
    $cb = [Win32+EnumProc]{
        param($h, $l)
        if (-not [Win32]::IsWindowVisible($h)) { return $true }
        $p = 0
        [void][Win32]::GetWindowThreadProcessId($h, [ref]$p)
        if ($p -ne $procId) { return $true }
        $r = New-Object Win32+RECT
        if (-not [Win32]::GetClientRect($h, [ref]$r)) { return $true }
        $w = $r.Right - $r.Left
        $hgt = $r.Bottom - $r.Top
        if ($w -le 0 -or $hgt -le 0) { return $true }
        $sb = New-Object System.Text.StringBuilder 256
        [void][Win32]::GetClassName($h, $sb, 256)
        $cls = $sb.ToString()
        if ($cls -match 'console') { return $true }
        $rank = 0
        if ($cls -match 'easyx') { $rank = 3 }
        elseif (($w * $hgt) -gt 500000) { $rank = 2 }
        $area = $w * $hgt
        if ($rank -gt $script:foundRank -or ($rank -eq $script:foundRank -and $rank -ge 2 -and $area -gt $script:bestArea)) {
            $script:found = $h
            $script:foundRank = $rank
            $script:bestArea = $area
        }
        return $true
    }
    [void][Win32]::EnumWindows($cb, [IntPtr]::Zero)
    return $script:found
}

function Wait-StableWindow([int]$procId, [int]$timeoutMs = 12000) {
    $sw = [Diagnostics.Stopwatch]::StartNew()
    while ($sw.ElapsedMilliseconds -lt $timeoutMs) {
        $h1 = Find-GameWindow $procId
        if ($h1 -ne [IntPtr]::Zero) {
            Start-Sleep -Milliseconds 350
            $h2 = Find-GameWindow $procId
            if ($h2 -ne [IntPtr]::Zero -and $h1 -eq $h2) {
                $r = New-Object Win32+RECT
                if ([Win32]::GetClientRect($h2, [ref]$r)) {
                    if (($r.Right - $r.Left) -gt 0 -and ($r.Bottom - $r.Top) -gt 0) { return $h2 }
                }
            }
        }
        Start-Sleep -Milliseconds 150
    }
    throw "no stable game window"
}

function Activate-Window($hwnd, [int]$procId) {
    try { [Microsoft.VisualBasic.Interaction]::AppActivate($procId) | Out-Null } catch { }
    [void][Win32]::ShowWindow($hwnd, 9)   # SW_RESTORE
    # keep the whole client area above the taskbar: move window to screen origin (physical px)
    [void][Win32]::SetWindowPos($hwnd, [IntPtr]::Zero, 0, 0, 0, 0, 0x0001)   # SWP_NOSIZE | HWND_TOP
    [void][Win32]::SetForegroundWindow($hwnd)
    Start-Sleep -Milliseconds 450
}

function Get-DpiScale($hwnd) {
    $cr = New-Object Win32+RECT
    [void][Win32]::GetClientRect($hwnd, [ref]$cr)
    $w = [double]($cr.Right - $cr.Left)
    if ($w -le 0) { return 1.0 }
    return $w / 1427.0   # EasyX logical width
}

function Move-Client($hwnd, [int]$x, [int]$y) {
    $scale = Get-DpiScale $hwnd
    $p = New-Object Win32+POINT
    $p.X = 0
    $p.Y = 0
    [void][Win32]::ClientToScreen($hwnd, [ref]$p)
    [void][Win32]::SetCursorPos($p.X + [int][Math]::Round($x * $scale), $p.Y + [int][Math]::Round($y * $scale))
    Start-Sleep -Milliseconds 250
}

function Click-Client($hwnd, [int]$x, [int]$y) {
    # EasyX window is DPI-unaware: client coords are virtual, screen coords are physical
    $scale = Get-DpiScale $hwnd
    $p = New-Object Win32+POINT
    $p.X = 0
    $p.Y = 0
    [void][Win32]::ClientToScreen($hwnd, [ref]$p)
    $sx = $p.X + [int][Math]::Round($x * $scale)
    $sy = $p.Y + [int][Math]::Round($y * $scale)
    [void][Win32]::SetCursorPos($sx, $sy)
    Start-Sleep -Milliseconds 300
    [Win32]::mouse_event(0x0002, 0, 0, 0, [UIntPtr]::Zero)   # MOUSEEVENTF_LEFTDOWN
    Start-Sleep -Milliseconds 120
    [Win32]::mouse_event(0x0004, 0, 0, 0, [UIntPtr]::Zero)   # MOUSEEVENTF_LEFTUP
    Start-Sleep -Milliseconds 400
}

function Capture-Client($hwnd, [string]$path, [int]$targetW = 1427, [int]$targetH = 830) {
    $cr = New-Object Win32+RECT
    [void][Win32]::GetClientRect($hwnd, [ref]$cr)
    $w = $cr.Right - $cr.Left
    $h = $cr.Bottom - $cr.Top
    if ($w -le 0 -or $h -le 0) { throw "bad client rect" }

    $p = New-Object Win32+POINT
    $p.X = 0
    $p.Y = 0
    [void][Win32]::ClientToScreen($hwnd, [ref]$p)

    $raw = New-Object System.Drawing.Bitmap($w, $h)
    $g = [System.Drawing.Graphics]::FromImage($raw)
    $g.CopyFromScreen($p.X, $p.Y, 0, 0, (New-Object System.Drawing.Size($w, $h)))
    $g.Dispose()

    $dst = New-Object System.Drawing.Bitmap($targetW, $targetH)
    $g2 = [System.Drawing.Graphics]::FromImage($dst)
    $g2.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
    $g2.DrawImage($raw, 0, 0, $targetW, $targetH)
    $g2.Dispose()
    $raw.Dispose()

    $dst.Save($path, [System.Drawing.Imaging.ImageFormat]::Png)
    $dst.Dispose()
    Write-Output ("saved " + $path + " (source " + $w + "x" + $h + ")")
}

$proc = Start-Process -FilePath $exe -WorkingDirectory $workdir -PassThru
try {
    Start-Sleep -Milliseconds 3000
    $h = Wait-StableWindow $proc.Id
    Activate-Window $h $proc.Id
    Start-Sleep -Milliseconds 900
    # force un-mute via the "+" volume button, then park the cursor on "start battle"
    Click-Client $h 1362 760
    Start-Sleep -Milliseconds 400
    Move-Client $h 714 355
    Start-Sleep -Milliseconds 700
    Capture-Client $h (Join-Path $outDir 'main_menu.png')

    # main menu -> team config
    Click-Client $h 714 435
    Start-Sleep -Milliseconds 1200
    $h = Wait-StableWindow $proc.Id
    Activate-Window $h $proc.Id
    Start-Sleep -Milliseconds 700
    Capture-Client $h (Join-Path $outDir 'team_config.png')

    # team config -> back to main menu
    Click-Client $h 1160 770
    Start-Sleep -Milliseconds 1200
    $h = Wait-StableWindow $proc.Id
    Activate-Window $h $proc.Id

    # main menu -> start battle -> pet choice player1 -> pet choice player2
    Click-Client $h 714 355
    Start-Sleep -Milliseconds 1200
    $h = Wait-StableWindow $proc.Id
    Activate-Window $h $proc.Id
    Click-Client $h 323 400
    Start-Sleep -Milliseconds 1200
    $h = Wait-StableWindow $proc.Id
    Activate-Window $h $proc.Id
    Click-Client $h 693 400
    Start-Sleep -Milliseconds 2600
    $h = Wait-StableWindow $proc.Id
    Activate-Window $h $proc.Id
    Start-Sleep -Milliseconds 600
    Capture-Client $h (Join-Path $outDir 'battle.png')

    # escape -> confirm -> result screen
    Click-Client $h 1380 600
    Start-Sleep -Milliseconds 900
    Click-Client $h 623 462
    Start-Sleep -Milliseconds 2200
    $h = Wait-StableWindow $proc.Id
    Activate-Window $h $proc.Id
    Start-Sleep -Milliseconds 700
    Capture-Client $h (Join-Path $outDir 'result.png')
}
finally {
    Start-Sleep -Milliseconds 600
    if (-not $proc.HasExited) { Stop-Process -Id $proc.Id -Force }
}
Write-Output "done"
