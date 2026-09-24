#!/usr/bin/env python3
"""Reproduce builds; never uploads or erases a device."""
import argparse, json, os, shutil, subprocess, sys
from pathlib import Path
ROOT=Path(__file__).resolve().parent
BASE='esp32:esp32:esp32s3:CDCOnBoot=default,FlashSize=16M,PSRAM=opi'
TFT='-DUSER_SETUP_LOADED -DILI9488_DRIVER -DTFT_MISO=-1 -DTFT_MOSI=11 -DTFT_SCLK=12 -DTFT_CS=10 -DTFT_DC=14 -DTFT_RST=21 -DLOAD_GLCD -DSPI_FREQUENCY=16000000'
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--install',action='store_true',help='Install pinned core and libraries; internet required')
p.add_argument('--target',choices=['firmware','checks','extra25','all'],default='firmware')
p.add_argument('--cli',default='arduino-cli')
p.add_argument('--config-file')
a=p.parse_args()
cli=shutil.which(a.cli)
if not cli:sys.exit('BUILD BLOCKED: arduino-cli is not installed or --cli path is invalid.')
base=[cli]+(['--config-file',a.config_file] if a.config_file else [])
if a.install:
 subprocess.run(base+['core','update-index','--additional-urls','https://espressif.github.io/arduino-esp32/package_esp32_index.json'],check=True)
 subprocess.run(base+['core','install','esp32:esp32@3.3.1','--additional-urls','https://espressif.github.io/arduino-esp32/package_esp32_index.json'],check=True)
 subprocess.run(base+['lib','update-index'],check=True)
 for lib in json.loads((ROOT/'dependencies.lock.json').read_text()):
  subprocess.run(base+['lib','install',lib['name']+'@'+lib['version']],check=True)
plan=[]
if a.target in ['firmware','all']:
 for profile in [0,1]:
  plan.append(('SPAVN' if profile else 'SPECTRE', 'firmware/spectre_s3',BASE,{'compiler.cpp.extra_flags':f'-DSPAVN_PROFILE={profile}'}))
if a.target in ['checks','all']:
 plan += [('common','compatibility/common',BASE+',PartitionScheme=huge_app',{k:'-DLV_CONF_SKIP' for k in ['compiler.cpp.extra_flags','compiler.c.extra_flags','compiler.S.extra_flags']}),
          ('async_web','compatibility/async_web',BASE,{}),
          ('usb_keyboard','compatibility/usb_keyboard',BASE+',USBMode=default',{}),
          ('tft_espi','compatibility/tft_espi',BASE,{'compiler.cpp.extra_flags':TFT}),
          ('idf_display','compatibility/idf_display',BASE,{}),
          ('speech','compatibility/speech',BASE+',PartitionScheme=esp_sr_16',{})]
if a.target in ['extra25','all']:
 for item in json.loads((ROOT/'additional25/catalog.json').read_text()):
  plan.append((f"extra{item['number']}",item['path'],BASE,{}))
results=[]
for name,sketch,fqbn,props in plan:
 cmd=base+['compile','--jobs','2','--fqbn',fqbn,'--libraries',str(ROOT/'vendor')]
 for key,value in props.items():cmd+=['--build-property',key+'='+value]
 if name in ['SPECTRE','SPAVN']:cmd+=['--output-dir',str(ROOT/'rebuilt'/name)]
 cmd+=[str(ROOT/sketch)]
 (ROOT/'rebuild-logs').mkdir(exist_ok=True)
 with (ROOT/'rebuild-logs'/f'{name}.log').open('w') as log:
  r=subprocess.run(cmd,stdout=log,stderr=subprocess.STDOUT)
 results.append(dict(target=name,exit_code=r.returncode,command=cmd))
 print(f'{name}: BUILD '+('PASS' if r.returncode==0 else 'FAIL'),flush=True)
 if r.returncode:
  print('\n'.join((ROOT/'rebuild-logs'/f'{name}.log').read_text().splitlines()[-16:]))
(ROOT/'rebuild-logs/results.json').write_text(json.dumps(results,indent=2))
sys.exit(any(r['exit_code'] for r in results))
