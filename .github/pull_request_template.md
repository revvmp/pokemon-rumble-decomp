Closes #

## Units

| Unit | Status | Functions matching |
| --- | --- | --- |
|  |  |  |

## Commands and results

```
python configure.py
.venv/Scripts/ninja
python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1
```

## Checklist

- [ ] Full build succeeds and verification exits 0
- [ ] Units marked `Matching` are 100% in objdiff
- [ ] Only my claimed ranges changed in `splits.txt` and `symbols.txt`
- [ ] No comments, docstrings, unit tests, or raw decompiler output
- [ ] Supporting evidence and validation results included
- [ ] No co-author trailers or tool attribution lines in commits or this description
