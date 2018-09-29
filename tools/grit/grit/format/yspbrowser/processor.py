# //ysp { localization }
import sys
import os

reload(sys)
sys.setdefaultencoding('utf8')

sys.path.append(os.path.join(os.path.dirname(__file__), '.'))
import default

def ProtectStrings(mapping, strings):
  num=0
  for prv in mapping.preserved:
    place_holder = '###_%d_###' % num
    if prv in strings:
      strings = strings.replace(prv, place_holder)
    num = num+1
  return strings

def RevertProtectedStrings(mapping, strings):
  num=0
  for prv in mapping.preserved:
    place_holder = '###_%d_###' % num
    if place_holder in strings:
      strings = strings.replace(place_holder, prv)
    num = num+1
  return strings

def Replace(mapping, strings):

  strings = ProtectStrings(mapping, strings)    # preserve strings

  # replace localizable strings priority 1
  for src, loc in mapping.strings_p1.iteritems():
    if src in strings:
      strings = strings.replace(src, loc)

  strings = ProtectStrings(mapping, strings)            # strings may be changed, we need re-protect it.

  # replace localizable strings priority 2
  for src, loc in mapping.strings_p2.iteritems():
    if src in strings:
      strings = strings.replace(src, loc)

  strings = ProtectStrings(mapping, strings)            # strings may be changed, we need re-protect it.

  # replace localizable strings priority 3
  for src, loc in mapping.strings_p3.iteritems():
    if src in strings:
      strings = strings.replace(src, loc)

  return RevertProtectedStrings(mapping, strings)    # unprotect strings

def Parse(strings, lang):
  if len(lang) == 0:
    return strings

  # output source strings for debugging
  #message_log = file('c:/xbrowser_translation.log', 'a')
  #if lang == "zh-TW":
  #  message_log.write('==== lang: ' + lang + ' ====\n' + strings + '\n')

  try:
    # Generally, argument |lang| is in 2 forms:
    # 1. From Chrome projects, it is 'zh-CN', 'en', 'es-419';
    # 2. From installer projects, it is 'ZH_CN', 'EN', 'ES_419';
    # mapping modules name is in form 'zh_cn', 'en', 'es_419';
    # We need normallize it for loading correct string mapping module.
    lang = lang.replace('-', '_').lower()
    mapping = __import__(str(lang))
    strings = Replace(mapping, strings)
    #if lang == "zh-TW":
    #  message_log.write('==== translated: ' + lang + ' ====\n' + strings + '\n')
  except ImportError:
    #print 'Use default translation for language:', lang
    strings = Replace(default, strings)
  #message_log.close()
  return strings
