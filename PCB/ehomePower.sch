<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="8.7.0">
<drawing>
<settings>
<setting alwaysvectorfont="no"/>
<setting verticaltext="up"/>
</settings>
<grid distance="0.1" unitdist="inch" unit="inch" style="lines" multiple="1" display="no" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers>
<layer number="1" name="Top" color="4" fill="1" visible="no" active="no"/>
<layer number="16" name="Bottom" color="1" fill="1" visible="no" active="no"/>
<layer number="17" name="Pads" color="2" fill="1" visible="no" active="no"/>
<layer number="18" name="Vias" color="2" fill="1" visible="no" active="no"/>
<layer number="19" name="Unrouted" color="6" fill="1" visible="no" active="no"/>
<layer number="20" name="Dimension" color="24" fill="1" visible="no" active="no"/>
<layer number="21" name="tPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="22" name="bPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="23" name="tOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="24" name="bOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="25" name="tNames" color="7" fill="1" visible="no" active="no"/>
<layer number="26" name="bNames" color="7" fill="1" visible="no" active="no"/>
<layer number="27" name="tValues" color="7" fill="1" visible="no" active="no"/>
<layer number="28" name="bValues" color="7" fill="1" visible="no" active="no"/>
<layer number="29" name="tStop" color="7" fill="3" visible="no" active="no"/>
<layer number="30" name="bStop" color="7" fill="6" visible="no" active="no"/>
<layer number="31" name="tCream" color="7" fill="4" visible="no" active="no"/>
<layer number="32" name="bCream" color="7" fill="5" visible="no" active="no"/>
<layer number="33" name="tFinish" color="6" fill="3" visible="no" active="no"/>
<layer number="34" name="bFinish" color="6" fill="6" visible="no" active="no"/>
<layer number="35" name="tGlue" color="7" fill="4" visible="no" active="no"/>
<layer number="36" name="bGlue" color="7" fill="5" visible="no" active="no"/>
<layer number="37" name="tTest" color="7" fill="1" visible="no" active="no"/>
<layer number="38" name="bTest" color="7" fill="1" visible="no" active="no"/>
<layer number="39" name="tKeepout" color="4" fill="11" visible="no" active="no"/>
<layer number="40" name="bKeepout" color="1" fill="11" visible="no" active="no"/>
<layer number="41" name="tRestrict" color="4" fill="10" visible="no" active="no"/>
<layer number="42" name="bRestrict" color="1" fill="10" visible="no" active="no"/>
<layer number="43" name="vRestrict" color="2" fill="10" visible="no" active="no"/>
<layer number="44" name="Drills" color="7" fill="1" visible="no" active="no"/>
<layer number="45" name="Holes" color="7" fill="1" visible="no" active="no"/>
<layer number="46" name="Milling" color="3" fill="1" visible="no" active="no"/>
<layer number="47" name="Measures" color="7" fill="1" visible="no" active="no"/>
<layer number="48" name="Document" color="7" fill="1" visible="no" active="no"/>
<layer number="49" name="Reference" color="7" fill="1" visible="no" active="no"/>
<layer number="51" name="tDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="52" name="bDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="88" name="SimResults" color="9" fill="1" visible="yes" active="yes"/>
<layer number="89" name="SimProbes" color="9" fill="1" visible="yes" active="yes"/>
<layer number="90" name="Modules" color="5" fill="1" visible="yes" active="yes"/>
<layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes"/>
<layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes"/>
<layer number="93" name="Pins" color="2" fill="1" visible="no" active="yes"/>
<layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes"/>
<layer number="95" name="Names" color="7" fill="1" visible="yes" active="yes"/>
<layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes"/>
<layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes"/>
<layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries>
<library name="con-wago-500" urn="urn:adsk.eagle:library:195">
<description>&lt;b&gt;Wago Screw Clamps&lt;/b&gt;&lt;p&gt;
Grid 5.00 mm&lt;p&gt;
&lt;author&gt;Created by librarian@cadsoft.de&lt;/author&gt;</description>
<packages>
<package name="W237-102" urn="urn:adsk.eagle:footprint:10676/1" library_version="1">
<description>&lt;b&gt;WAGO SCREW CLAMP&lt;/b&gt;</description>
<wire x1="-3.491" y1="-2.286" x2="-1.484" y2="-0.279" width="0.254" layer="51"/>
<wire x1="1.488" y1="-2.261" x2="3.469" y2="-0.254" width="0.254" layer="51"/>
<wire x1="-4.989" y1="-5.461" x2="4.993" y2="-5.461" width="0.1524" layer="21"/>
<wire x1="4.993" y1="3.734" x2="4.993" y2="3.531" width="0.1524" layer="21"/>
<wire x1="4.993" y1="3.734" x2="-4.989" y2="3.734" width="0.1524" layer="21"/>
<wire x1="-4.989" y1="-5.461" x2="-4.989" y2="-3.073" width="0.1524" layer="21"/>
<wire x1="-4.989" y1="-3.073" x2="-3.389" y2="-3.073" width="0.1524" layer="21"/>
<wire x1="-3.389" y1="-3.073" x2="-1.611" y2="-3.073" width="0.1524" layer="51"/>
<wire x1="-1.611" y1="-3.073" x2="1.615" y2="-3.073" width="0.1524" layer="21"/>
<wire x1="3.393" y1="-3.073" x2="4.993" y2="-3.073" width="0.1524" layer="21"/>
<wire x1="-4.989" y1="-3.073" x2="-4.989" y2="3.531" width="0.1524" layer="21"/>
<wire x1="4.993" y1="-3.073" x2="4.993" y2="-5.461" width="0.1524" layer="21"/>
<wire x1="-4.989" y1="3.531" x2="4.993" y2="3.531" width="0.1524" layer="21"/>
<wire x1="-4.989" y1="3.531" x2="-4.989" y2="3.734" width="0.1524" layer="21"/>
<wire x1="4.993" y1="3.531" x2="4.993" y2="-3.073" width="0.1524" layer="21"/>
<wire x1="1.615" y1="-3.073" x2="3.393" y2="-3.073" width="0.1524" layer="51"/>
<circle x="-2.5" y="-1.27" radius="1.4986" width="0.1524" layer="51"/>
<circle x="-2.5" y="2.2098" radius="0.508" width="0.1524" layer="21"/>
<circle x="2.5038" y="-1.27" radius="1.4986" width="0.1524" layer="51"/>
<circle x="2.5038" y="2.2098" radius="0.508" width="0.1524" layer="21"/>
<pad name="1" x="-2.5" y="-1.27" drill="1.1938" shape="long" rot="R90"/>
<pad name="2" x="2.5" y="-1.27" drill="1.1938" shape="long" rot="R90"/>
<text x="-5.04" y="-7.62" size="1.27" layer="27" ratio="10">&gt;VALUE</text>
<text x="-3.8462" y="-5.0038" size="1.27" layer="25" ratio="10">&gt;NAME</text>
<text x="-4.532" y="0.635" size="1.27" layer="21" ratio="10">1</text>
<text x="0.421" y="0.635" size="1.27" layer="21" ratio="10">2</text>
</package>
</packages>
<packages3d>
<package3d name="W237-102" urn="urn:adsk.eagle:package:10688/1" type="box" library_version="1">
<description>WAGO SCREW CLAMP</description>
<packageinstances>
<packageinstance name="W237-102"/>
</packageinstances>
</package3d>
</packages3d>
<symbols>
<symbol name="KL" urn="urn:adsk.eagle:symbol:10675/1" library_version="1">
<circle x="1.27" y="0" radius="1.27" width="0.254" layer="94"/>
<text x="0" y="0.889" size="1.778" layer="95" rot="R180">&gt;NAME</text>
<pin name="KL" x="5.08" y="0" visible="off" length="short" direction="pas" rot="R180"/>
</symbol>
<symbol name="KL+V" urn="urn:adsk.eagle:symbol:10673/1" library_version="1">
<circle x="1.27" y="0" radius="1.27" width="0.254" layer="94"/>
<text x="-2.54" y="-3.683" size="1.778" layer="96">&gt;VALUE</text>
<text x="0" y="0.889" size="1.778" layer="95" rot="R180">&gt;NAME</text>
<pin name="KL" x="5.08" y="0" visible="off" length="short" direction="pas" rot="R180"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="W237-102" urn="urn:adsk.eagle:component:10702/1" prefix="X" uservalue="yes" library_version="1">
<description>&lt;b&gt;WAGO SCREW CLAMP&lt;/b&gt;</description>
<gates>
<gate name="-1" symbol="KL" x="0" y="5.08" addlevel="always"/>
<gate name="-2" symbol="KL+V" x="0" y="0" addlevel="always"/>
</gates>
<devices>
<device name="" package="W237-102">
<connects>
<connect gate="-1" pin="KL" pad="1"/>
<connect gate="-2" pin="KL" pad="2"/>
</connects>
<package3dinstances>
<package3dinstance package3d_urn="urn:adsk.eagle:package:10688/1"/>
</package3dinstances>
<technologies>
<technology name="">
<attribute name="MF" value="" constant="no"/>
<attribute name="MPN" value="237-102" constant="no"/>
<attribute name="OC_FARNELL" value="unknown" constant="no"/>
<attribute name="OC_NEWARK" value="70K9898" constant="no"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="personal">
<packages>
<package name="AM2DS">
<pad name="P$1" x="5.14" y="5.1" drill="0.6" shape="long" rot="R90"/>
<pad name="P$2" x="10.245" y="5.075" drill="0.6" shape="long" rot="R90"/>
<pad name="P$3" x="15.22" y="5.05" drill="0.6" shape="long" rot="R90"/>
<pad name="P$4" x="18.06" y="5.025" drill="0.6" shape="long" rot="R90"/>
<circle x="18.311" y="2.191" radius="0.5" width="0.127" layer="21"/>
<wire x1="0.024" y1="6.953" x2="0.024" y2="-0.079" width="0.127" layer="51"/>
<wire x1="0.024" y1="-0.079" x2="19.438" y2="-0.079" width="0.127" layer="51"/>
<wire x1="19.438" y1="-0.079" x2="19.438" y2="6.953" width="0.127" layer="51"/>
<wire x1="19.438" y1="6.953" x2="0.024" y2="6.953" width="0.127" layer="51"/>
</package>
</packages>
<symbols>
<symbol name="AM2DS">
<pin name="VOUT" x="-5.12" y="6.58" length="middle" direction="in" rot="R270"/>
<pin name="GND" x="-0.5" y="6.58" length="middle" direction="in" rot="R270"/>
<pin name="GNDIN" x="4.12" y="6.58" length="middle" direction="in" rot="R270"/>
<pin name="VIN" x="6.7" y="6.58" length="middle" direction="in" rot="R270"/>
<wire x1="-8.5" y1="1.492" x2="-8.5" y2="-8.064" width="0.254" layer="94"/>
<wire x1="-8.5" y1="-8.064" x2="8" y2="-8.064" width="0.254" layer="94"/>
<wire x1="8" y1="-8.064" x2="8" y2="1.492" width="0.254" layer="94"/>
<wire x1="8" y1="1.492" x2="-8.5" y2="1.492" width="0.254" layer="94"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="AM2DS">
<gates>
<gate name="G$1" symbol="AM2DS" x="0" y="0"/>
</gates>
<devices>
<device name="" package="AM2DS">
<connects>
<connect gate="G$1" pin="GND" pad="P$2"/>
<connect gate="G$1" pin="GNDIN" pad="P$3"/>
<connect gate="G$1" pin="VIN" pad="P$4"/>
<connect gate="G$1" pin="VOUT" pad="P$1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="supply2" urn="urn:adsk.eagle:library:372">
<description>&lt;b&gt;Supply Symbols&lt;/b&gt;&lt;p&gt;
GND, VCC, 0V, +5V, -5V, etc.&lt;p&gt;
Please keep in mind, that these devices are necessary for the
automatic wiring of the supply signals.&lt;p&gt;
The pin name defined in the symbol is identical to the net which is to be wired automatically.&lt;p&gt;
In this library the device names are the same as the pin names of the symbols, therefore the correct signal names appear next to the supply symbols in the schematic.&lt;p&gt;
&lt;author&gt;Created by librarian@cadsoft.de&lt;/author&gt;</description>
<packages>
</packages>
<symbols>
<symbol name="VCC" urn="urn:adsk.eagle:symbol:26997/1" library_version="2">
<circle x="0" y="1.27" radius="1.27" width="0.254" layer="94"/>
<text x="-1.905" y="3.175" size="1.778" layer="96">&gt;VALUE</text>
<pin name="VCC" x="0" y="-2.54" visible="off" length="short" direction="sup" rot="R90"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="VCC" urn="urn:adsk.eagle:component:27060/1" prefix="SUPPLY" library_version="2">
<description>&lt;b&gt;SUPPLY SYMBOL&lt;/b&gt;</description>
<gates>
<gate name="G$1" symbol="VCC" x="0" y="0"/>
</gates>
<devices>
<device name="">
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="supply1" urn="urn:adsk.eagle:library:371">
<description>&lt;b&gt;Supply Symbols&lt;/b&gt;&lt;p&gt;
 GND, VCC, 0V, +5V, -5V, etc.&lt;p&gt;
 Please keep in mind, that these devices are necessary for the
 automatic wiring of the supply signals.&lt;p&gt;
 The pin name defined in the symbol is identical to the net which is to be wired automatically.&lt;p&gt;
 In this library the device names are the same as the pin names of the symbols, therefore the correct signal names appear next to the supply symbols in the schematic.&lt;p&gt;
 &lt;author&gt;Created by librarian@cadsoft.de&lt;/author&gt;</description>
<packages>
</packages>
<symbols>
<symbol name="GND" urn="urn:adsk.eagle:symbol:26925/1" library_version="1">
<wire x1="-1.905" y1="0" x2="1.905" y2="0" width="0.254" layer="94"/>
<text x="-2.54" y="-2.54" size="1.778" layer="96">&gt;VALUE</text>
<pin name="GND" x="0" y="2.54" visible="off" length="short" direction="sup" rot="R270"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="GND" urn="urn:adsk.eagle:component:26954/1" prefix="GND" library_version="1">
<description>&lt;b&gt;SUPPLY SYMBOL&lt;/b&gt;</description>
<gates>
<gate name="1" symbol="GND" x="0" y="0"/>
</gates>
<devices>
<device name="">
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
</libraries>
<attributes>
</attributes>
<variantdefs>
</variantdefs>
<classes>
<class number="0" name="default" width="0" drill="0">
</class>
</classes>
<parts>
<part name="X1" library="con-wago-500" library_urn="urn:adsk.eagle:library:195" deviceset="W237-102" device="" package3d_urn="urn:adsk.eagle:package:10688/1"/>
<part name="X7" library="con-wago-500" library_urn="urn:adsk.eagle:library:195" deviceset="W237-102" device="" package3d_urn="urn:adsk.eagle:package:10688/1"/>
<part name="X6" library="con-wago-500" library_urn="urn:adsk.eagle:library:195" deviceset="W237-102" device="" package3d_urn="urn:adsk.eagle:package:10688/1"/>
<part name="X8" library="con-wago-500" library_urn="urn:adsk.eagle:library:195" deviceset="W237-102" device="" package3d_urn="urn:adsk.eagle:package:10688/1"/>
<part name="X9" library="con-wago-500" library_urn="urn:adsk.eagle:library:195" deviceset="W237-102" device="" package3d_urn="urn:adsk.eagle:package:10688/1"/>
<part name="X11" library="con-wago-500" library_urn="urn:adsk.eagle:library:195" deviceset="W237-102" device="" package3d_urn="urn:adsk.eagle:package:10688/1"/>
<part name="X12" library="con-wago-500" library_urn="urn:adsk.eagle:library:195" deviceset="W237-102" device="" package3d_urn="urn:adsk.eagle:package:10688/1"/>
<part name="X13" library="con-wago-500" library_urn="urn:adsk.eagle:library:195" deviceset="W237-102" device="" package3d_urn="urn:adsk.eagle:package:10688/1"/>
<part name="X10" library="con-wago-500" library_urn="urn:adsk.eagle:library:195" deviceset="W237-102" device="" package3d_urn="urn:adsk.eagle:package:10688/1"/>
<part name="X2" library="con-wago-500" library_urn="urn:adsk.eagle:library:195" deviceset="W237-102" device="" package3d_urn="urn:adsk.eagle:package:10688/1"/>
<part name="X4" library="con-wago-500" library_urn="urn:adsk.eagle:library:195" deviceset="W237-102" device="" package3d_urn="urn:adsk.eagle:package:10688/1"/>
<part name="X14" library="con-wago-500" library_urn="urn:adsk.eagle:library:195" deviceset="W237-102" device="" package3d_urn="urn:adsk.eagle:package:10688/1"/>
<part name="X15" library="con-wago-500" library_urn="urn:adsk.eagle:library:195" deviceset="W237-102" device="" package3d_urn="urn:adsk.eagle:package:10688/1"/>
<part name="U$2" library="personal" deviceset="AM2DS" device=""/>
<part name="SUPPLY1" library="supply2" library_urn="urn:adsk.eagle:library:372" deviceset="VCC" device=""/>
<part name="SUPPLY2" library="supply2" library_urn="urn:adsk.eagle:library:372" deviceset="VCC" device=""/>
<part name="SUPPLY3" library="supply2" library_urn="urn:adsk.eagle:library:372" deviceset="VCC" device=""/>
<part name="SUPPLY5" library="supply2" library_urn="urn:adsk.eagle:library:372" deviceset="VCC" device=""/>
<part name="SUPPLY7" library="supply2" library_urn="urn:adsk.eagle:library:372" deviceset="VCC" device=""/>
<part name="SUPPLY8" library="supply2" library_urn="urn:adsk.eagle:library:372" deviceset="VCC" device=""/>
<part name="SUPPLY9" library="supply2" library_urn="urn:adsk.eagle:library:372" deviceset="VCC" device=""/>
<part name="SUPPLY10" library="supply2" library_urn="urn:adsk.eagle:library:372" deviceset="VCC" device=""/>
<part name="SUPPLY11" library="supply2" library_urn="urn:adsk.eagle:library:372" deviceset="VCC" device=""/>
<part name="SUPPLY12" library="supply2" library_urn="urn:adsk.eagle:library:372" deviceset="VCC" device=""/>
<part name="SUPPLY13" library="supply2" library_urn="urn:adsk.eagle:library:372" deviceset="VCC" device=""/>
<part name="SUPPLY14" library="supply2" library_urn="urn:adsk.eagle:library:372" deviceset="VCC" device=""/>
<part name="GND1" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="GND" device=""/>
<part name="GND2" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="GND" device=""/>
<part name="GND4" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="GND" device=""/>
<part name="GND6" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="GND" device=""/>
<part name="GND7" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="GND" device=""/>
<part name="GND8" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="GND" device=""/>
<part name="GND9" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="GND" device=""/>
<part name="GND10" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="GND" device=""/>
<part name="GND11" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="GND" device=""/>
<part name="GND12" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="GND" device=""/>
<part name="GND13" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="GND" device=""/>
<part name="GND14" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="GND" device=""/>
<part name="GND15" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="GND" device=""/>
<part name="X5" library="con-wago-500" library_urn="urn:adsk.eagle:library:195" deviceset="W237-102" device="" package3d_urn="urn:adsk.eagle:package:10688/1"/>
<part name="SUPPLY6" library="supply2" library_urn="urn:adsk.eagle:library:372" deviceset="VCC" device=""/>
<part name="GND5" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="GND" device=""/>
<part name="X3" library="con-wago-500" library_urn="urn:adsk.eagle:library:195" deviceset="W237-102" device="" package3d_urn="urn:adsk.eagle:package:10688/1"/>
<part name="SUPPLY4" library="supply2" library_urn="urn:adsk.eagle:library:372" deviceset="VCC" device=""/>
<part name="GND3" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="GND" device=""/>
</parts>
<sheets>
<sheet>
<plain>
</plain>
<instances>
<instance part="X1" gate="-1" x="-20.32" y="81.28" rot="R90"/>
<instance part="X1" gate="-2" x="-15.24" y="81.28" rot="R90"/>
<instance part="X7" gate="-1" x="58.42" y="81.28" rot="R90"/>
<instance part="X7" gate="-2" x="63.5" y="81.28" rot="R90"/>
<instance part="X6" gate="-1" x="45.72" y="81.28" rot="R90"/>
<instance part="X6" gate="-2" x="50.8" y="81.28" rot="R90"/>
<instance part="X8" gate="-1" x="-22.86" y="50.8" rot="R90"/>
<instance part="X8" gate="-2" x="-17.78" y="50.8" rot="R90"/>
<instance part="X9" gate="-1" x="-10.16" y="50.8" rot="R90"/>
<instance part="X9" gate="-2" x="-5.08" y="50.8" rot="R90"/>
<instance part="X11" gate="-1" x="15.24" y="50.8" rot="R90"/>
<instance part="X11" gate="-2" x="20.32" y="50.8" rot="R90"/>
<instance part="X12" gate="-1" x="27.94" y="50.8" rot="R90"/>
<instance part="X12" gate="-2" x="33.02" y="50.8" rot="R90"/>
<instance part="X13" gate="-1" x="40.64" y="50.8" rot="R90"/>
<instance part="X13" gate="-2" x="45.72" y="50.8" rot="R90"/>
<instance part="X10" gate="-1" x="2.54" y="50.8" rot="R90"/>
<instance part="X10" gate="-2" x="7.62" y="50.8" rot="R90"/>
<instance part="X2" gate="-1" x="-7.62" y="81.28" rot="R90"/>
<instance part="X2" gate="-2" x="0" y="81.28" rot="R90"/>
<instance part="X4" gate="-1" x="20.32" y="81.28" rot="R90"/>
<instance part="X4" gate="-2" x="25.4" y="81.28" rot="R90"/>
<instance part="X14" gate="-1" x="53.34" y="50.8" rot="R90"/>
<instance part="X14" gate="-2" x="60.96" y="50.8" rot="R90"/>
<instance part="X15" gate="-1" x="106.68" y="88.9" rot="R90"/>
<instance part="X15" gate="-2" x="111.76" y="88.9" rot="R90"/>
<instance part="U$2" gate="G$1" x="81.28" y="111.76"/>
<instance part="SUPPLY1" gate="G$1" x="104.14" y="99.06" rot="R90"/>
<instance part="SUPPLY2" gate="G$1" x="-20.32" y="93.98"/>
<instance part="SUPPLY3" gate="G$1" x="-7.62" y="91.44"/>
<instance part="SUPPLY5" gate="G$1" x="20.32" y="91.44"/>
<instance part="SUPPLY7" gate="G$1" x="45.72" y="91.44"/>
<instance part="SUPPLY8" gate="G$1" x="-22.86" y="58.42"/>
<instance part="SUPPLY9" gate="G$1" x="-10.16" y="58.42"/>
<instance part="SUPPLY10" gate="G$1" x="2.54" y="58.42"/>
<instance part="SUPPLY11" gate="G$1" x="15.24" y="58.42"/>
<instance part="SUPPLY12" gate="G$1" x="27.94" y="58.42"/>
<instance part="SUPPLY13" gate="G$1" x="40.64" y="58.42"/>
<instance part="SUPPLY14" gate="G$1" x="53.34" y="58.42"/>
<instance part="GND1" gate="1" x="-15.24" y="91.44" rot="R180"/>
<instance part="GND2" gate="1" x="0" y="91.44" rot="R180"/>
<instance part="GND4" gate="1" x="25.4" y="91.44" rot="R180"/>
<instance part="GND6" gate="1" x="50.8" y="91.44" rot="R180"/>
<instance part="GND7" gate="1" x="58.42" y="91.44" rot="R180"/>
<instance part="GND8" gate="1" x="-17.78" y="58.42" rot="R180"/>
<instance part="GND9" gate="1" x="-5.08" y="58.42" rot="R180"/>
<instance part="GND10" gate="1" x="7.62" y="58.42" rot="R180"/>
<instance part="GND11" gate="1" x="20.32" y="58.42" rot="R180"/>
<instance part="GND12" gate="1" x="33.02" y="58.42" rot="R180"/>
<instance part="GND13" gate="1" x="45.72" y="58.42" rot="R180"/>
<instance part="GND14" gate="1" x="60.96" y="58.42" rot="R180"/>
<instance part="GND15" gate="1" x="86.36" y="129.54" rot="R180"/>
<instance part="X5" gate="-1" x="33.02" y="81.28" rot="R90"/>
<instance part="X5" gate="-2" x="38.1" y="81.28" rot="R90"/>
<instance part="SUPPLY6" gate="G$1" x="33.02" y="91.44"/>
<instance part="GND5" gate="1" x="38.1" y="91.44" rot="R180"/>
<instance part="X3" gate="-1" x="7.62" y="81.28" rot="R90"/>
<instance part="X3" gate="-2" x="12.7" y="81.28" rot="R90"/>
<instance part="SUPPLY4" gate="G$1" x="7.62" y="91.44"/>
<instance part="GND3" gate="1" x="12.7" y="91.44" rot="R180"/>
</instances>
<busses>
</busses>
<nets>
<net name="N$1" class="0">
<segment>
<pinref part="U$2" gate="G$1" pin="VOUT"/>
<wire x1="76.16" y1="118.34" x2="76.16" y2="127" width="0.1524" layer="91"/>
<wire x1="76.16" y1="127" x2="76.2" y2="127" width="0.1524" layer="91"/>
<wire x1="76.16" y1="127" x2="63.5" y2="127" width="0.1524" layer="91"/>
<pinref part="X7" gate="-2" pin="KL"/>
<wire x1="63.5" y1="127" x2="63.5" y2="86.36" width="0.1524" layer="91"/>
</segment>
</net>
<net name="VCC" class="0">
<segment>
<pinref part="X8" gate="-1" pin="KL"/>
<pinref part="SUPPLY8" gate="G$1" pin="VCC"/>
</segment>
<segment>
<pinref part="X9" gate="-1" pin="KL"/>
<pinref part="SUPPLY9" gate="G$1" pin="VCC"/>
</segment>
<segment>
<pinref part="X10" gate="-1" pin="KL"/>
<pinref part="SUPPLY10" gate="G$1" pin="VCC"/>
</segment>
<segment>
<pinref part="X11" gate="-1" pin="KL"/>
<pinref part="SUPPLY11" gate="G$1" pin="VCC"/>
</segment>
<segment>
<pinref part="X12" gate="-1" pin="KL"/>
<pinref part="SUPPLY12" gate="G$1" pin="VCC"/>
</segment>
<segment>
<pinref part="X13" gate="-1" pin="KL"/>
<pinref part="SUPPLY13" gate="G$1" pin="VCC"/>
</segment>
<segment>
<pinref part="X14" gate="-1" pin="KL"/>
<pinref part="SUPPLY14" gate="G$1" pin="VCC"/>
</segment>
<segment>
<pinref part="X1" gate="-1" pin="KL"/>
<pinref part="SUPPLY2" gate="G$1" pin="VCC"/>
<wire x1="-20.32" y1="91.44" x2="-20.32" y2="86.36" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="X15" gate="-1" pin="KL"/>
<wire x1="106.68" y1="121.92" x2="106.68" y2="99.06" width="0.1524" layer="91"/>
<wire x1="106.68" y1="99.06" x2="106.68" y2="93.98" width="0.1524" layer="91"/>
<wire x1="91.44" y1="121.92" x2="106.68" y2="121.92" width="0.1524" layer="91"/>
<pinref part="U$2" gate="G$1" pin="VIN"/>
<wire x1="87.98" y1="118.34" x2="91.44" y2="121.8" width="0.1524" layer="91"/>
<wire x1="91.44" y1="121.8" x2="91.44" y2="121.92" width="0.1524" layer="91"/>
<pinref part="SUPPLY1" gate="G$1" pin="VCC"/>
<junction x="106.68" y="99.06"/>
</segment>
<segment>
<pinref part="SUPPLY3" gate="G$1" pin="VCC"/>
<pinref part="X2" gate="-1" pin="KL"/>
<wire x1="-7.62" y1="88.9" x2="-7.62" y2="86.36" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="SUPPLY5" gate="G$1" pin="VCC"/>
<pinref part="X4" gate="-1" pin="KL"/>
<wire x1="20.32" y1="88.9" x2="20.32" y2="86.36" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="SUPPLY7" gate="G$1" pin="VCC"/>
<pinref part="X6" gate="-1" pin="KL"/>
<wire x1="45.72" y1="88.9" x2="45.72" y2="86.36" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="SUPPLY6" gate="G$1" pin="VCC"/>
<pinref part="X5" gate="-1" pin="KL"/>
<wire x1="33.02" y1="88.9" x2="33.02" y2="86.36" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="SUPPLY4" gate="G$1" pin="VCC"/>
<pinref part="X3" gate="-1" pin="KL"/>
<wire x1="7.62" y1="88.9" x2="7.62" y2="86.36" width="0.1524" layer="91"/>
</segment>
</net>
<net name="GND" class="0">
<segment>
<pinref part="X8" gate="-2" pin="KL"/>
<pinref part="GND8" gate="1" pin="GND"/>
</segment>
<segment>
<pinref part="X9" gate="-2" pin="KL"/>
<pinref part="GND9" gate="1" pin="GND"/>
</segment>
<segment>
<pinref part="X10" gate="-2" pin="KL"/>
<pinref part="GND10" gate="1" pin="GND"/>
</segment>
<segment>
<pinref part="X11" gate="-2" pin="KL"/>
<pinref part="GND11" gate="1" pin="GND"/>
</segment>
<segment>
<pinref part="X12" gate="-2" pin="KL"/>
<pinref part="GND12" gate="1" pin="GND"/>
</segment>
<segment>
<pinref part="X13" gate="-2" pin="KL"/>
<pinref part="GND13" gate="1" pin="GND"/>
</segment>
<segment>
<pinref part="X14" gate="-2" pin="KL"/>
<pinref part="GND14" gate="1" pin="GND"/>
</segment>
<segment>
<pinref part="X15" gate="-2" pin="KL"/>
<wire x1="111.76" y1="93.98" x2="111.76" y2="127" width="0.1524" layer="91"/>
<wire x1="111.76" y1="127" x2="86.36" y2="127" width="0.1524" layer="91"/>
<pinref part="U$2" gate="G$1" pin="GNDIN"/>
<wire x1="86.36" y1="127" x2="85.4" y2="126.04" width="0.1524" layer="91"/>
<wire x1="85.4" y1="126.04" x2="85.4" y2="118.34" width="0.1524" layer="91"/>
<pinref part="U$2" gate="G$1" pin="GND"/>
<wire x1="80.78" y1="118.34" x2="80.78" y2="127" width="0.1524" layer="91"/>
<wire x1="80.78" y1="127" x2="86.36" y2="127" width="0.1524" layer="91"/>
<junction x="86.36" y="127"/>
<pinref part="GND15" gate="1" pin="GND"/>
</segment>
<segment>
<pinref part="GND1" gate="1" pin="GND"/>
<pinref part="X1" gate="-2" pin="KL"/>
<wire x1="-15.24" y1="88.9" x2="-15.24" y2="86.36" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="GND2" gate="1" pin="GND"/>
<pinref part="X2" gate="-2" pin="KL"/>
<wire x1="0" y1="88.9" x2="0" y2="86.36" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="GND4" gate="1" pin="GND"/>
<pinref part="X4" gate="-2" pin="KL"/>
<wire x1="25.4" y1="88.9" x2="25.4" y2="86.36" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="GND6" gate="1" pin="GND"/>
<pinref part="X6" gate="-2" pin="KL"/>
<wire x1="50.8" y1="88.9" x2="50.8" y2="86.36" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="GND7" gate="1" pin="GND"/>
<pinref part="X7" gate="-1" pin="KL"/>
<wire x1="58.42" y1="88.9" x2="58.42" y2="86.36" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="GND5" gate="1" pin="GND"/>
<pinref part="X5" gate="-2" pin="KL"/>
<wire x1="38.1" y1="88.9" x2="38.1" y2="86.36" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="GND3" gate="1" pin="GND"/>
<pinref part="X3" gate="-2" pin="KL"/>
<wire x1="12.7" y1="88.9" x2="12.7" y2="86.36" width="0.1524" layer="91"/>
</segment>
</net>
</nets>
</sheet>
</sheets>
</schematic>
</drawing>
<compatibility>
<note version="8.2" severity="warning">
Since Version 8.2, EAGLE supports online libraries. The ids
of those online libraries will not be understood (or retained)
with this version.
</note>
<note version="8.3" severity="warning">
Since Version 8.3, EAGLE supports URNs for individual library
assets (packages, symbols, and devices). The URNs of those assets
will not be understood (or retained) with this version.
</note>
<note version="8.3" severity="warning">
Since Version 8.3, EAGLE supports the association of 3D packages
with devices in libraries, schematics, and board files. Those 3D
packages will not be understood (or retained) with this version.
</note>
</compatibility>
</eagle>
