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

$roundData = Get-Content -Raw -LiteralPath $roundFile | ConvertFrom-Json

if ($CommitMessage -eq "") {
    $CommitMessage = "feat: auto sync round $RoundNum"
}

$jsonlFile = "AI开发审核_曾天乐_图书信息管理系统.jsonl"

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

$existingCommit = git log --oneline --all | Select-String -Pattern "^$($roundData.commit_hash)" -SimpleMatch
if (-not $existingCommit) {
    git add "round/round${RoundNum}.json" $jsonlFile
    git commit -m $CommitMessage
}

$remoteUrl = git remote get-url origin 2>$null
if ($remoteUrl) {
    git push origin HEAD
    Write-Host "Pushed to $remoteUrl"
} else {
    Write-Warning "No remote configured; commit created locally only."
}

Write-Host "Sync round $RoundNum completed."
