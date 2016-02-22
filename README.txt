array_to_dom: a simple PHP extension to convert nested arrays into DOMDocument
Version: 1.5
Rabotyahoff Alexandr, http://ra-project.net, https://github.com/Rabotyahoff/array_to_dom

Based on dom_varimport by Dmitry Koterov, http://en.dklab.ru/lib/dom_varimport/, https://github.com/DmitryKoterov/dom_varimport
License: PHP License 3.01

Support php7


MOTIVATION
----------

Some of projects use XSLT as a templating engine. To build XML for such engines,
we need a very fast and memory efficient way to convert PHP nested arrays and
objects into DOMDocument object. Also, produced XML should be straight-forward
and as simple as it could be (BTW XMLRPC, SOAP and other XML-based formats are quite
slopy in this case). So array_to_dom has been introduced: it produces DOMDocument
from a nested array near 20 times faster than a hand-made code in native PHP
(1 MB XML with thousands of nodes could be generated in 1-2 ms).


HOW TO BUILD
------------

phpize
./configure
make
make install  # or copy modules/array_to_dom.so manually
phpize --clean


SYNOPSIS
--------

The code:

  $arr=array();
  $arr[]='first item';
  $arr[]='second item';
  $arr['bool_true']=true;
  $arr['bool_false']=false;
  $arr['old_name']=array('*'=>'new_name', '.'=>'val');//don't replace node name
  $arr[]=array('*'=>'real_new_name', '.'=>'val');//set node name = 'real_new_name'
  $arr['item']=array('*'=>'real_new_name2', '.'=>'val');//set node name = 'real_new_name2'
  $arr['user']='vanya';
  $arr['user2']=array('name'=>'vanya', 'lastname'=>'ivanov');
  $obj=(object)array("prop" => "val");
  $arr['obj']=$obj;
  $arr['with_attr']=array('@attr'=>'val', '.'=>'val', '@int'=>666, '@int2'=>'777', '@int2'=>'776', '@double'=>1.2345, '@bool'=>true, '@bool'=>false );//set attributes

  $doc = new DOMDocument('1.0', 'UTF-8');
  $doc->formatOutput = true;
  array_to_dom( $doc, $arr, "res");// "res" is optional, defaults to "root"
  echo $doc->saveXML();

prints:

  <?xml version="1.0" encoding="UTF-8"?>
  <res>
    <item>first item</item>
    <item>second item</item>
    <bool_true>1</bool_true>
    <bool_false/>
    <old_name>val</old_name>
    <real_new_name>val</real_new_name>
    <real_new_name2>val</real_new_name2>
    <user>vanya</user>
    <user2>
      <name>vanya</name>
      <lastname>иванов</lastname>
    </user2>
    <obj>
      <prop>val</prop>
    </obj>
    <with_attr attr="val" int="666" int2="776" double="1.2345" bool="">val</with_attr>
  </res>
