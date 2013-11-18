#!/usr/bin/env ruby  -wKU
$:.unshift File.expand_path("../vendor/osc-ruby/lib", __FILE__)
require 'osc-ruby'

load(File.absolute_path("#{File.dirname(__FILE__)}/util.rb"))
client = OSC::Client.new('127.0.0.1', 4556)
client.send(OSC::Message.new("/d_loadDir", sp_synthdefs_path))
