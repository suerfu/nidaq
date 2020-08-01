
import config as cfg
import os


cfgfile = 'nidaq.cfg'
cmd = 'polaris --time 10 --cfg ' +  cfgfile

os.system( cmd )

