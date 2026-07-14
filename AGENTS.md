After completing code modifications for each interaction round, you MUST execute the sync script to push changes to GitHub and append the round record to the JSONL audit file:
```powershell
.\sync_to_github.ps1
```
Use `.\sync_to_github.ps1 -RoundNum <N>` if a specific round number is needed. This applies the round data from `round/round<N>.json` to `AI开发审核_曾天乐_图书信息管理系统.jsonl` and commits/pushes to GitHub.
