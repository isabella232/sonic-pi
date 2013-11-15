#!/usr/bin/env ruby
$:.unshift File.expand_path("../vendor/osc-ruby/lib", __FILE__)
load(File.absolute_path("#{File.dirname(__FILE__)}/util.rb"))

require 'osc-ruby'
STDOUT.sync

SYNTH_GROUP = 1

client = OSC::Client.new('127.0.0.1', 4556)
client.send(OSC::Message.new("g_freeAll", SYNTH_GROUP))
