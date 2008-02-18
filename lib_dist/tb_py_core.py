"""
Python Library for Trollbot

Author: comcor@DALnet
"""

import imp
import trollbot
import sys

NETWORKS = {}

"""
TrollbotNetworkInterface

Marshals access to the trollbot module for the given network.
Each script loaded will have a reference to an instance of this
object for the network it is attached to
"""
class TrollbotNetworkInterface:

   def __init__(self, network):
      self.network = network #ptr to TB net struct
      self.modules = {}
   #end __init__

   def load_module(self, name):
      ok = False 
      fp, pathname, description = imp.find_module(name)

      try:
         module = imp.load_module(name, fp, pathname, description)
         if module is not None:
            #pass module a reference to us
            module.trollbot = self
            #initialize our module
            module.load()

            #save the module for access later
            self.modules[name] = module
            ok = True
      finally:
         if fp:
            fp.close
      return ok
   #end load_module

   def bind(self,*args):
      trollbot.bind(self.network, *args)

   def putserv(self, *args):
      trollbot.putserv(self.network, *args)

   def call_method(self,callback, *args):
      #get module name
      (module, method) = callback.split(".")
      if self.modules.has_key(module):
         cb = getattr(self.modules[module], method)
         cb(*args)

#end TrollbotNetworkInterface


##
## INTERNAL FUNCTIONS
##
## Please don't call these unless you know what you are doing.
## These are meant to be called through the Python C/API
##

def __TB_init_network(network):
   if not NETWORKS.has_key(network):
      NETWORKS[network] = TrollbotNetworkInterface(network)
#end init_network

def __TB_load_module(network, module):
   if NETWORKS.has_key(network):
      return NETWORKS[network].load_module(module)
   #end if
   return False
#end __TB_load_module

def __TB_call_method(network, callback, *args):
   if NETWORKS.has_key(network):
      return NETWORKS[network].call_method(callback, *args)
   return False
#end __TB_call_method

def __TB_add_path(path):
   sys.path.append(path)
#end __TB_add_path
