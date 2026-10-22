import json,re,subprocess,sys,collections
unit=sys.argv[1]
apply='--apply' in sys.argv
j=subprocess.run(['python','tools/re/unitdiff.py','--json','--no-build',unit],capture_output=True,text=True).stdout
d=json.loads(j)
funcs=[s['name'] for s in d['left']['symbols'] if s.get('kind')=='SYMBOL_FUNCTION' and s.get('match_percent',100)<100]
votes=collections.defaultdict(collections.Counter)
sfx=r'@(ha|l|h|sda21|sda2|sda)$'
for f in funcs:
    out=subprocess.run(['python','tools/re/unitdiff.py','--no-build',unit,'--function',f],capture_output=True,text=True).stdout
    for l in out.splitlines():
        m=re.match(r'\s+0x[0-9a-f]+\s+[|ro]?\s*(.*)$',l)
        if not m: continue
        cols=re.split(r'\s{3,}',m.group(1).strip())
        if len(cols)!=2: continue
        lt=[x for x in re.split(r',\s+|\s+|[()]',cols[0]) if x]; rt=[x for x in re.split(r',\s+|\s+|[()]',cols[1]) if x]
        if len(lt)!=len(rt) or lt[0]!=rt[0] or lt[0].startswith('b'): continue
        for a,b in zip(lt[1:],rt[1:]):
            na=re.sub(sfx,'',a); nb=re.sub(sfx,'',b)
            if na==nb or not re.match(r'^[A-Za-z_]',na) or not re.match(r'^[A-Za-z_]',nb): continue
            if re.match(r'^(r|f)\d+$',na) or re.match(r'^(r|f)\d+$',nb): continue
            if na.startswith('...') or nb.startswith('...'): continue
            if not ((na.startswith('lbl_') or na.startswith(('__vt__','__RTTI__','g','s')) ) and not nb.startswith(('lbl_','...'))): continue
            votes[na][nb]+=1
p='config/wpse-v1/symbols.txt'
lines=open(p,encoding='utf-8').read().split('\n')
byname=collections.defaultdict(list)
for i,l in enumerate(lines):
    m=re.match(r'^(\S+) = (\.\w+):(0x[0-9A-F]+);',l)
    if m: byname[m.group(1)].append(i)
ren={}
for a,c in votes.items():
    b,n=c.most_common(1)[0]
    if len(c)>1: print('multi',a[:50],[(k[:40],v) for k,v in c.items()])
    if a not in byname or len(byname[a])!=1: print('skipA',a[:60]); continue
    ren[byname[a][0]]=b
vals=list(ren.values())
cnt=0
for i,b in ren.items():
    if vals.count(b)>1: print('dup target',b[:60]); continue
    ex=[k for k in byname.get(b,[]) if k not in ren]
    if ex: print('exists elsewhere',b[:60],lines[ex[0]].split(' = ')[1][:30]); continue
    print(lines[i].split(' = ')[0][:50],'->',b[:70]); cnt+=1
    if apply: lines[i]=b+' = '+lines[i].split(' = ',1)[1]
if apply: open(p,'w',encoding='utf-8').write('\n'.join(lines))
print(cnt)
