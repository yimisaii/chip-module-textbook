param([string]$Gcc = 'gcc.exe')

$ErrorActionPreference = 'Stop'
$project = (Resolve-Path (Join-Path $PSScriptRoot '../project')).Path
$root = Join-Path $project 'MDK-ARM'
$xmlPath = Join-Path $root 'GPIO_EXTI.uvprojx'
[xml]$xml = Get-Content -Raw -LiteralPath $xmlPath
$target = $xml.Project.Targets.Target
if ($target.TargetName -ne 'GPIO_EXTI' -or $target.uAC6 -ne '1') {
    throw 'Expected the GPIO_EXTI Arm Compiler 6 target'
}
$defines = $target.TargetOption.TargetArmAds.Cads.VariousControls.Define.Split(',')
if ('STM32U575xx' -notin $defines -or 'USE_HAL_DRIVER' -notin $defines) {
    throw 'Missing STM32U575xx or USE_HAL_DRIVER'
}
$includes = $target.TargetOption.TargetArmAds.Cads.VariousControls.IncludePath.Split(';')
$includes = $includes | ForEach-Object { '-I' + (Resolve-Path -LiteralPath (Join-Path $root $_)).Path }
$sources = $target.Groups.Group.Files.File | ForEach-Object {
    (Resolve-Path -LiteralPath (Join-Path $root $_.FilePath)).Path
}
$scatter = (Resolve-Path -LiteralPath (Join-Path $root $target.TargetOption.TargetArmAds.LDads.ScatterFile)).Path
$startup = @($sources | Where-Object { $_.EndsWith('.s') })
if ($startup.Count -ne 1 -or $sources.Count -ne (@($sources | Select-Object -Unique)).Count) {
    throw 'Expected one startup file and unique sources'
}
if ((Get-Content -Raw -LiteralPath $startup[0]) -notmatch 'DCD\s+EXTI13_IRQHandler' -or
    (Get-Content -Raw -LiteralPath $startup[0]) -notmatch 'DCD\s+SysTick_Handler' -or
    (Get-Content -Raw -LiteralPath $scatter) -notmatch 'startup_stm32u575xx\.o\s+\(RESET, \+First\)') {
    throw 'Missing startup vector or RESET placement'
}
"PASS XML, $($sources.Count) source paths, $($includes.Count) include paths, scatter and vector symbols"

$gccCommand = (Get-Command $Gcc).Source
$env:Path = "$(Split-Path $gccCommand);$env:Path"
& $gccCommand --version | Select-Object -First 1
& $gccCommand -dumpmachine
$c = $sources | Where-Object { $_.EndsWith('.c') }
$defines = $defines | ForEach-Object { '-D' + $_ }
& $gccCommand -std=c99 -fsyntax-only -Wall -Wextra -Werror -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast @defines @includes @c
if ($LASTEXITCODE -ne 0) { throw 'HAL1 C syntax check failed' }
"PASS C syntax against the bundled HAL1 and CMSIS headers ($($c.Count) translation units)"

$work = New-Item -Force -ItemType Directory -Path (Join-Path $PSScriptRoot 'work')
$test = Join-Path $work.FullName 'test_button.exe'
& $gccCommand -std=c99 -Wall -Wextra -Werror "-I$project/Core" "$PSScriptRoot/test_button.c" "$project/Core/button.c" -o $test
if ($LASTEXITCODE -ne 0) { throw 'Host test build failed' }
& $test
if ($LASTEXITCODE -ne 0) { throw 'Host test failed' }
