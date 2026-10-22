import json,re,subprocess,sys,collections
unit=sys.argv[1]
lo=int(sys.argv[2],16);hi=int(sys.argv[3],16)
apply='--apply' in sys.argv
j=subprocess.run(['python','tools/re/unitdiff.py','--json','--no-build',unit],capture_output=True,text=True).stdout
d=json.loads(j)
funcs=[s['name'] for s in d['left']['symbols'] if s.get('kind')=='SYMBOL_FUNCTION' and s.get('match_percent',100)<100]
votes=collections.defaultdict(collections.Counter)
def tmpl(n): return 'std' in n or 'Metrowerks' in n
for f in funcs:
    out=subprocess.run(['python','tools/re/unitdiff.py','--no-build',unit,'--function',f],capture_output=True,text=True).stdout
    for l in out.splitlines():
        m=re.match(r'\s+0x[0-9a-f]+\s+[|ro]?\s*bl\s+(\S+)\s+bl\s+(\S+)\s*$',l)
        if m and m.group(1)!=m.group(2) and (tmpl(m.group(1)) or m.group(1).startswith(('fn_','dtor_'))) and tmpl(m.group(2)):
            votes[m.group(1)][m.group(2)]+=1
p='config/wpse-v1/symbols.txt'
lines=open(p,encoding='utf-8').read().split('\n')
byname=collections.defaultdict(list)
for i,l in enumerate(lines):
    m=re.match(r'^(\S+) = \.text:(0x[0-9A-F]+);',l)
    if m: byname[m.group(1)].append((i,int(m.group(2),16)))
ren={}
for a,c in votes.items():
    b,n=c.most_common(1)[0]
    if len(c)>1: print('multi',a[:50],[(k[:40],v) for k,v in c.items()])
    if a not in byname or len(byname[a])!=1: print('skipA',a[:60]); continue
    i,ad=byname[a][0]
    if not lo<=ad<hi: print('range skip',a[:60],hex(ad)); continue
    ren[i]=b
vals=list(ren.values())
for i,b in ren.items():
    if vals.count(b)>1: print('dup target',b[:60]); continue
    ex=[k for k in byname.get(b,[]) if k[0] not in ren]
    if ex: print('exists elsewhere',b[:60]); continue
    print(lines[i].split(' = ')[0][:50],'->',b[:70])
    if apply: lines[i]=b+' = '+lines[i].split(' = ',1)[1]
if apply: open(p,'w',encoding='utf-8').write('\n'.join(lines))
