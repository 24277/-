param(
    [int]$RoundNum = -1,
    [string]$CommitMessage = ""
)

$ErrorActionPreference = "Stop"

$projectRoot = Split-Path -LiteralPath $MyInvocation.MyCommand.Path
Set-Location -LiteralPath $projectRoot

if ($RoundNum -eq -1) {
    $existingRounds = Get-ChildItem -Path "round" -Filter "round*.json" | ForEach-Object {
        if ($_.Name -match 'round(\d+)\.json') { [int]$matches[1] }
    } | Sort-Object
    $RoundNum = if ($existingRounds.Count -gt 0) { $existingRounds[-1] } else { 0 }
}

$roundFile = "round/round$RoundNum.json"
if (-not (Test-Path $roundFile)) {
    Write-Error "Round file not found: $roundFile"
    exit 1
}

$roundRaw = [System.IO.File]::ReadAllText($roundFile)
try {
    $roundData = $roundRaw | ConvertFrom-Json
} catch {
    Write-Error "Invalid JSON in $roundFile"
    exit 1
}

if ($CommitMessage -eq "") {
    $CommitMessage = "feat: auto sync round $RoundNum"
}

$jsonlFile = "AI开发审核_曾天乐_图书信息管理系统.jsonl"

$existingIds = if (Test-Path $jsonlFile) { Get-Content -LiteralPath $jsonlFile | ForEach-Object { try { ($_ | ConvertFrom-Json).round_id } catch {} } } else { @() }
if ($roundData.round_id -in $existingIds) {
    Write-Warning "round_id $($roundData.round_id) already exists in JSONL, skipping append."
} else {
    $jsonlEntry = @{
        round_id      = $roundData.round_id
        prompt_content = $roundData.prompt_content
        modify_diff   = $roundData.modify_diff
        commit_hash   = $roundData.commit_hash
        modify_time   = $roundData.modify_time
        agent_type    = $roundData.agent_type
        dev_language  = $roundData.dev_language
    } | ConvertTo-Json -Compress

    Add-Content -LiteralPath $jsonlFile -Value $jsonlEntry
}

$hashPrefix = $roundData.commit_hash.Substring(0, [Math]::Min(7, $roundData.commit_hash.Length))
$existingCommit = git log --oneline --all 2>$null | Select-String -Pattern "^$hashPrefix" -SimpleMatch
if (-not $existingCommit) {
    git add "round/round${RoundNum}.json" $jsonlFile
    git commit -m $CommitMessage
}

$remoteName = $null
git remote -v | Select-String -Pattern "^(a|origin)\s+" | ForEach-Object {
    $parts = $_ -split '\s+'
    if (-not $remoteName) { $remoteName = $parts[0] }
}
if ($remoteName) {
    git push "${remoteName}" HEAD
    Write-Host "Pushed to ${remoteName}"
} else {
    Write-Warning "No remote configured; commit created locally only."
}

Write-Host "Sync round $RoundNum completed."
