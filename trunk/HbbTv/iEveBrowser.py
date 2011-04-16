#killall rcS; killall enigma2; mount -tvfat /dev/sda1 /root; export LD_LIBRARY_PATH=/root/dfb/lib:$LD_LIBRARY_PATH; enigma2

import os
import subprocess
import sys
from Tools.Directories import resolveFilename, SCOPE_PLUGINS

ctypes = None
try:
  print "Loading ctypes"
  ctypes = __import__("_ctypes")
  print "Loading ctypes - Done"
except Exception, ex:
  print "Loading ctypes - Failed (%s)" % ex
  ctypes = None

class c_int(ctypes._SimpleCData):
  _type_ = "i"

class CFunctionType(ctypes.CFuncPtr):
  _argtypes_ = (c_int, )
  _restype_ = c_int
  _flags_ = ctypes.FUNCFLAG_CDECL

gIEveBrowser = None

def _iEveBrowser__evtInfo(type):
  print "_iEveBrowser__evtInfo:", type
  if gIEveBrowser is not None:
    gIEveBrowser._evtInfo(type)
  return 0



class iEveBrowser():
  
  eveBrowser = None
  evtInfo = None

  
  def __init__(self):
    #os.system('export DFBARGS="pixelformat=ARGB,no-cursor,bg-none')
    os.environ["DFBARGS"] = "pixelformat=ARGB,no-cursor,bg-none,no-linux-input-grab"


    
    try:
      print "Loading libeve-browser.so.0.0.0"
      
      # Add Library Path to Python path
      sys.path.append("/root/dfb/lib")
      
      # Change Library Search Path
      if os.environ.has_key("LD_LIBRARY_PATH"):
        os.environ["LD_LIBRARY_PATH"] = "/root/dfb/lib:" + os.environ["LD_LIBRARY_PATH"]
      else:
        os.putenv('LD_LIBRARY_PATH', "/root/dfb/lib")
      #os.system("export LD_LIBRARY_PATH=/root/dfb/lib:$LD_LIBRARY_PATH")
      
      lib = resolveFilename(SCOPE_PLUGINS) + "/SystemPlugins/HbbTv/lib/libeve-browser.so.0.0.0"
      self.eveBrowser = ctypes.dlopen(lib, ctypes.RTLD_GLOBAL)
      print "Loading libeve-browser.so.0.0.0 - Done"
    except Exception, ex:
      print "Loading libeve-browser.so.0.0.0 - Failed (%s)" % ex
      return
    
    try:
      print "Registering functions"
      self._setDimension = ctypes.dlsym(self.eveBrowser, "_Z12setDimensionii")
      self._loadEveBrowser = ctypes.dlsym(self.eveBrowser, "_Z14loadEveBrowserv")
      self._unloadEveBrowser = ctypes.dlsym(self.eveBrowser, "_Z16unloadEveBrowserv")
      self._loadPage = ctypes.dlsym(self.eveBrowser, "_Z8loadPagePc")
      self._show = ctypes.dlsym(self.eveBrowser, "_Z4showv")
      self._hide = ctypes.dlsym(self.eveBrowser, "_Z4hidev")
      self._keyPress = ctypes.dlsym(self.eveBrowser, "_Z8keyPressPci")
      self._setCallback = ctypes.dlsym(self.eveBrowser, "_Z11setCallbackPFiiE")
      print "Registering functions - Done"
    except Exception, ex:
      print "Registering functions - Failed (%s)" % ex
      return
    
    try:
      print "Registering callback"

      self._EVTFUNC = CFunctionType(__evtInfo)
      ctypes.call_function(self._setCallback, (self._EVTFUNC, ))
      print "Registering callback - Done"
    except Exception, ex:
      print "Registering callback - Failed (%s)" % ex
      return
    
    global gIEveBrowser
    gIEveBrowser = self
    return
  
  ###
  # Framebuffer dimension
  def setDimension(self, w, h):
    if self.eveBrowser is not None:
      ctypes.call_function(self._setDimension, (w, h, ))
  
  ###
  # Load the webpage
  # Will not display it if hidden
  def loadPage(self, url):
    if self.eveBrowser is not None:
      ctypes.call_function(self._loadPage, (url, ))
  
  ###
  # Loads the browser
  # Will not display it if hidden
  def loadEveBrowser(self):
    if self.eveBrowser is not None:
      ctypes.call_function(self._loadEveBrowser, ())
  
  ###
  # Unloads the browser, give me my mem back
  # Will not display it if hidden
  def unloadEveBrowser(self):
    if self.eveBrowser is not None:
      ctypes.call_function(self._unloadEveBrowser, ())
  
  ###
  # Display browser
  def show(self):
    if self.eveBrowser is not None:
      ctypes.call_function(self._show, ())
  
  ###
  # Hide browser
  def hide(self):
    if self.eveBrowser is not None:
      ctypes.call_function(self._hide, ())
  
  KEY_TYPE_PRESS   = 0
  KEY_TYPE_RELEASE = 1
  KEY_TYPE_PRELL   = 2
  
  ###
  # Routs keypresses to the browser
  # type is one of the above KEY_TYPE_*
  def keyPress(self, key, type):
    if self.eveBrowser is not None:
      ctypes.call_function(self._keyPress, (key, type, ))
  
  
  EVT_EVE_BROWSER_LOADED = 0
  EVT_PAGE_LOADED = 1

  EVT_PIG_CHANGED = 2
  
  EVT_C_O_BIND_TO_CURRENT_CHANNEL = 100
  
  ###
  # Called when webpage has been loaded
  # There are differnt possible approches, either e2 displays the red button, or the browser
  def _evtInfo(self, type):
    print "_evtInfo:", type
    if type == self.EVT_EVE_BROWSER_LOADED:
      pass
    elif type == self.EVT_PAGE_LOADED:
      pass
    elif type == self.EVT_PIG_CHANGED:
      pass
    else:
      pass
    
    if self.evtInfo is not None:
      self.evtInfo(type)
  
  def setEvtInfoCallback(self, fnc):
    self.evtInfo = fnc
  
  def createLibraryLinks():
    libs = []
    libs.append("libatk-1.0.so.0")
    libs.append("libcairo.so.2")
    libs.append("libdirect-1.4.so.0")
    libs.append("libdirectfb-1.4.so.0")
    libs.append("libenchant.so.1")
    libs.append("libfusion-1.4.so.0")
    libs.append("libgailutil.so.18")
    libs.append("libgcrypt.so.11")
    libs.append("libgdk-directfb-2.0.so.0")
    libs.append("libgdk-x11-2.0.so.0")
    libs.append("libgdk_pixbuf-2.0.so.0")
    libs.append("libgio-2.0.so.0")
    libs.append("libglib-2.0.so.0")
    libs.append("libgmodule-2.0.so.0")
    libs.append("libgnutls.so.26")
    libs.append("libgobject-2.0.so.0")
    libs.append("libgpg-error.so.0")
    libs.append("libgthread-2.0.so.0")
    libs.append("libgtk-directfb-2.0.so.0")
    libs.append("libicudata.so.40")
    libs.append("libicui18n.so.40")
    libs.append("libicuuc.so.40")
    libs.append("libpango-1.0.so.0")
    libs.append("libpangocairo-1.0.so.0")
    libs.append("libpangoft2-1.0.so.0")
    libs.append("libpixman-1.so.0")
    libs.append("libsoup-2.4.so.1")
    libs.append("libsqlite3.so.0")
    libs.append("libsysfs.so.2")
    libs.append("libwebkit-1.0.so.2")
    
    for libname in libs:
      subprocess.Popen(("ln", "-s", "/root/dfb/lib/" + libname, "/usr/lib/" + libname, ))