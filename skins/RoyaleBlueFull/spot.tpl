<html><head>
<style>
/* FTD4Ever template - Omdat FTD in gedachten voor altijd bij ons blijft
 * Skin Design: foorsight & PeRKoniX ~~ Updates: Wantos
 * Template is uit c:\program files\ftd\cache(Wantos) - voor een groot deel gebaseerd op de aangepaste code van Copycat
 * Ported & extra afbeeldingen, qt.css en buttons Ypsos
 * met dank aan Ep Woody & Copycat (Spotlite Nostalgie skin)
 * Deze actie staat los van het FTD team
 */

body
{
	background: #40617b;
	font: normal 10px/12px verdana, arial, helvetica, sans-serif !important;
	margin: 0px;
	padding: 0px;
}

td
{
	font: normal 10px/12px verdana, arial, helvetica, sans-serif !important;
}

table
{
	table-layout: auto;
white-space: normal;}

img
{
vertical-align: top;
border: none;
padding-right: 0px;
padding-left: 0px;}

input,
select
{
	background: #fff;
	border: 1px solid #666;
	color: #333;
	font: normal 11px/11px verdana, arial, helvetica, sans-serif !important;
}

.clipboard
{
	border-bottom: 1px dotted #000 !important;
	cursor: hand !important;
}

.spotter
{
  color: blueviolet;
  font-weight: bolder;
}

.friend
{
  color: chartreuse;
  font-weight: bolder;
}

#search_box
{
	background-color: #fff !important;
	border: 1px solid #999 !important;
	font: normal 10px/12px verdana, arial, helvetica, sans-serif !important;
	padding: 0px;
	position: absolute;
	visibility: hidden;
}

#search_box_copy
{
	cursor: hand;
}

.search_box_td
{
	padding: 2px 7px 2px 7px;
}

.nopadding td
{
	padding-right: 0px !important;
}

.nopadding td table,
.nopadding td table td
{
	padding-right: auto !important;
}

.profile,
.profile td
{
	font: normal 11px/12px verdana, arial, helvetica, sans-serif !important;
}

.profile td
{
	border-bottom: 1px solid #aaa;
}

/**
* Headers
*/

.header
{
	color: #fff;
	font: normal 18px/19px verdana, arial, helvetica, sans-serif !important;
	padding: 0px 0px 0px 8px;
	background-color: #00FF66;
	background-image: url(headerbg.png);

}

.header_text
{
	color: #fff;
	font: normal 18px/19px verdana, arial, helvetica, sans-serif !important;
}

.catheader
{
	background: #ECFFF7 url('catbg.png');
	border-left: 1px solid #000000;
	border-top: 1px solid #000000;
	border-right: 1px solid #000000;
	color: #fff;
	font: normal 12px/14px verdana, arial, helvetica, sans-serif;
	letter-spacing: 1px !important;
	padding: 3px 3px 3px 12px;
}

.subheader
{
	background: url('subheader.png') no-repeat;
	padding-top: 3px;
}

.subheader_text,
.subheader_text a:active,
.subheader_text a:hover,
.subheader_text a:visited
{
	color: #fff !important;
	font: normal 10px/13px verdana, arial, helvetica, sans-serif;
}

.subheader_text a:link
{
	color: #fff !important;
	font: normal 10px/13px verdana, arial, helvetica, sans-serif;
}

.sublength
{
	background: url('subheaderbg.png') repeat-x;
	color: #fff !important;
	font: normal 10px/10px verdana, arial, helvetica, sans-serif;
	padding-top: 6px !important;
}

.sublength a
{
	color: #fff !important;
	font: normal 10px/10px verdana, arial, helvetica, sans-serif;
}

.fh,
.fh td,
.fh2,
.fh2 td
{
	background: url('frheader.png') !important;
	color: #fff;
	padding: 1px 3px 5px 3px !important;
	
}

.fh a,
.fh2 a
{
	color: #fafafa !important;
}

/**
* Footer
*/

.footer
{
	background: #40617b;
	color: #fff;
	font: normal 10px/14px verdana, arial, helvetica, sans-serif;
	padding: 4px 0px 20px 0px;
	text-align: center;
}

.list td
{
	letter-spacing: -1px !important;
}

/**
* Tables
*/

.subtable
{
	font-size: 1px;
	height: 1px !important;
}

.filestable
{
	color: #666;
	font: normal 10px/14px verdana, arial, helvetica, sans-serif;
	text-align: center;
	width: 100%;
}

.filestable td
{
	padding: 2px 0px 2px 0px;
}

.ft,
.ft td
{
	background: #fff !imporant;
	background-position: center center;
	background-repeat: no-repeat;
	color: #333;
	font: normal 11px/14px verdana, arial, helvetica, sans-serif !important;
	padding: 10px !important;
	padding-top: 5px !important;
}

.sft,
.sft tr,
.sft td
{
	background: url('frbody.png');
	color: #999;
	font: normal 10px/14px verdana, arial, helvetica, sans-serif !important;
	padding: 2px !important;
	padding-right: 10px !important;
}

/**
* Boxes
*/

.filebox
{
	background: #fff;
	padding: 5px;
}

.infobox
{
	background: #fff url('lightbg.png') repeat-x;
	width: 100%;
}

.infobox td
{
	padding: 3px;
	font: normal 11px/14px verdana, arial, helvetica, sans-serif !important;
}

.infobox td td
{
	padding: 1px 1px 1px 9px;
	font: normal 10px/14px verdana, arial, helvetica, sans-serif !important;
}

.infobox td td div
{
	font: bold 12px/14px verdana, arial, helvetica, sans-serif !important;
	text-decoration: none !important;
}

.infobox td td td
{
	padding: 0px;
	font: normal 10px/14px verdana, arial, helvetica, sans-serif !important;
}

.sigbox
{
	background: #fff url('lightbg4.png') no-repeat 5px 5px;
	border-bottom: 1px solid #333;
	border-top: 1px solid #333;
	font: normal 10px/14px verdana, arial, helvetica, sans-serif !important;
	padding: 17px;

}

.annbox
{
	background: url(lightbg2.png) repeat-x;
	color: #dd0000 !important;
	font: bold 10px/14px verdana, arial, helvetica, sans-serif !important;
	padding: 10px;
	margin-bottom: 5px;

}

.quote
{
	background: #fff url('./quote_back.gif') no-repeat right top;
	border: 1px solid #999 !important;
	font: italic 11px/14px verdana, arial, helvetica, sans-serif !important;
	padding: 5px 55px 5px 5px !important;
	margin-top: 5px;
	margin-bottom: 20px;
}

.quotehdr
{
	font: italic 11px/14px verdana, arial, helvetica, sans-serif !important;
	padding-left: 0px;
	text-decoration: underline;
}

.paroverviewbl
{
	background: #fff;
	border: 1px solid #999;
	color: #fff !important;
	font: normal 12px/14px verdana, arial, helvetica, sans-serif !important;
	letter-spacing: 0px !important;

}

#fo div
{
	border: 1px solid #999;
	color: #666 !important;
	font: normal 10px/14px verdana, arial, helvetica, sans-serif !important;
	padding: 2px !important;
}

.clblue
{
}

.clgreen
{
}

.clgray
{
}

#forumtab
{
	width: 0px !important;
}

.tab0
{
	background: url(tab0.gif);
	color: #fff !important;
	font: normal 10px/14px verdana, arial, helvetica, sans-serif !important;
	height: 16px;
	margin-left: 2px;
	padding: 2px 4px 0px 1px;
	text-align: center;
	width: 65px;
}

.tab1
{
	background: url(tab0.gif);
	color: #fff !important;
	font: bold 11px/14px verdana, arial, helvetica, sans-serif !important;
	height: 16px;
	margin-left: 2px;
	padding: 1px 4px 0px 1px;
	text-align: center;
	width: 65px;
}

.tab0 a,
.tab1 a
{
	color: #fff !important;
}

.overlay
{
	position: absolute;
	background-color: #FFFFEE;
	color: #61615A;
	background-image: url(overlaybg.jpg);
	border: 1px solid #B0B0A4;
	border-right-color: #585852;
	border-bottom-color: #585852;
	filter: alpha(opacity=0);
	font-family: Trebuchet MS;
	font-size: 12px;
	padding-left: 5px;
	padding-right: 5px;
}

a:link,a:active,a:visited
{
	text-decoration: none;
	color: #163657;
}

a:hover
{
	color: #FF0000;
}

.main
{
	width: 100%;
}

.subtable2
{
	background-image: url(subcatbg2.png);
	width: 100%;
	font-size: 8px;
	border-left: 1px solid #000000;
	border-bottom: 1px solid #000000;
	border-right: 1px solid #000000;
}

.bar
{
	width:100%;
	font-family: Verdana;
	font-size: 11px;
	background-image: url(bar.png);
	border-left: 1px solid #23568A;	border-right: 1px solid #23568A;
}

.infotable
{

}

.repostheader
{
	cursor: hand;
	font-weight: bold;
	background-color: #BABEFF;
	background-image: url(detail.png);
	background-repeat: repeat-x;
	padding-bottom:1px;
	font-size: 10px;
	color: #404484;
}

.blockheader
{
	cursor: hand;
	font-weight: bold;
	background-color: #BABEFF;
	background-image: url(detail2.png);
	background-repeat: repeat-x;
	padding-bottom:1px;
	font-size: 10px;
	color: #83746B;
}
		
.reposttable
{

	color: #000000;

	padding-left: 5px;
	width: 100%;
	font-size: 10px;
	font-family: trebuchet ms;
}

.mh
{
	font-size: 10px;
	background-color: #BDE2E8;
	background-image:url(frheader2.png);
	color: #5E7073;
	padding: 1px;
	padding-top: 0px;
	padding-left: 5px;
}

.mt
{
	background-color: #F6F1EA;
	background-image:url(frbody2.png);
	color: #000000;
	padding: 5px;
}


.fh2
{
	font-size: 10px;
	color: #705E42;
	padding: 1px;
	padding-top: 0px;
	padding-bottom: 5px;
	padding-left: 5px;
}

blockquote
{
	background-color: #FFFCFA;
	padding: 3px;
	font-size: 11px;
	font-family: Verdana, Helvetica, Arial, sans-serif;
	line-height: 11px;
	margin-left: 40px;
	border: 2px outset #FFF7F0;
	color: #6B5D4D;
}

.forum
{
	font-size: 11px;
	font-family: verdana;
	width: 100%;
	text-align: center;
	xborder-top: 1px solid #887860;
	xborder-left: 1px solid #887860;
	xborder-right: 1px solid #887860;

}

.subheader2
{

	background-image: url(subheader2.png);
	background-repeat: no-repeat;

}

.sublength
{
	background-image: url(subheaderbg.png);	
	background-repeat: repeat-x;
	padding-top: 6px;
	color: #204F7F;
}

.shtext
{
	vertical-align: top;
	font-family: verdana;
	font-size: 10px;

}

.mainform
{
	background-image: url(mainbg.png);
}

.quotehdr
{
	padding-left: 0px;
	text-decoration: underline;
}

/* Class voor NZB button 10-11-11 */
.nzb
{
	background:url('headerbg.png') repeat-x;
	border:none;
	border:0px;
	margin:0px;
	padding:0px;
	font: 67.5% "Lucida Sans Unicode", "Bitstream Vera Sans", "Trebuchet Unicode MS", "Lucida Grande", Verdana, Helvetica, sans-serif;
	font-size:14px;
	padding-top: 0em;
	padding-bottom: 0em;
	padding-left: 2px;
	padding-right: 2px;
	width: 36px;
}
	
.nzb ul
{
	height:41px;
	list-style:none;
	margin:0;
	padding:0;
}
	
.nzb li
{
	float:left;
	padding:0px;
}

.nzb li a
{
	background:#333333 url("nzb_off.gif") bottom no-repeat;
	color:#cccccc;
	display:block;
	font-weight:normal;
	line-height:41px;
	margin:0px;
	padding: 0px 15px 0px 15px;
	text-align:center;
	text-decoration:none;
}
		
.nzb li a:hover, .nzb ul li:hover a
{
	background: #2580a2 url("nzb_on.gif") bottom center no-repeat;
	color: #EEEEEE;
	text-decoration:none;
}

.nzb li ul
{
	display:none;
	height:auto;
	padding: 0px 0px 0px 3px;
	margin:0px;
	border:0px;
	position:absolute;
	width:160px;
	z-index: 400
}

.nzb li:hover ul
{
	display:block;
		
}

.nzb li li 
{
	background: url('nzb_bg_li.png') repeat-x;
	float:none;
	border:1px;
	margin:0px;
	padding:0px;
	width:160px;
}

.nzb li:hover li a
{
	background:none;
}

.nzb li ul a
{
	display:block;
	height:41px;
	font-size:12px;
	font-style:normal;
	margin:0px;
	padding:0px 10px 0px 15px;
	text-align:left;
}

.nzb li ul a:hover, .nzb li ul li:hover a
{
	background:#2580a2 url('headerbg.png') center left repeat;
	color: #ffffff;
	font-weight:bold;
}

.nzb p
{
	clear:left;
}

/* Class voor search button 11-11-11 */
.search
{
	background:url('b1-1.png') repeat;
	border:none;
	border:0px;
	margin:0px;
	padding:0px;
	font: 67.5% "Lucida Sans Unicode", "Bitstream Vera Sans", "Trebuchet Unicode MS", "Lucida Grande", Verdana, Helvetica, sans-serif;
	font-size:14px;
	width: 30px;
}

.search ul
{
	height:41px;
	list-style:none;
	margin:0;
	padding:0;
}

.search li
{
	float:left;
	padding:0px;
}

.search li a
{
	background:#3455A2 url('b1-search.png') bottom no-repeat;
	color:#cccccc;
	display:block;
	font-weight:normal;
	line-height:41px;
	margin:0px;
	padding: 0px 15px 0px 5px;  /* top - left - bottom - right */
	text-align:center;
	text-decoration:none;
		
}

.search li a:hover, .search ul li:hover a
{
	background: #2580a2 url('b1-search2.png') bottom center no-repeat;
	color:#000000;
	text-decoration:none;
}

.search li ul
{
	display:none;
	height:auto;
	padding: 0px 0px 0px 3px;
	margin:0px;
	border:0px;
	position:absolute;
	width:30px;
	z-index: 500
}

.search li:hover ul
{
	display:block;
		
}
	
.search li li 
{
	background:#3455A2 url('header1.png') repeat;
	list-style: none
	float:none;
	border:1px;
	margin:0px;
	padding:0px;
	width:30px;
}

.search li:hover li a
{
	background:#3455A2 url('subheaderbg.png') repeat;
	
		
}

	
.search li ul a
{
	display:block;
	height:10px;
	font-size:12px;
	font-style:normal;
	margin:0px;
	padding:12px 0px 0px 0px; /* top - left - bottom - right */
	text-align:left;
}

.search li ul a:hover, .search li ul li:hover a
{

	color: #ffffff;
	font-weight:bold;
}




.search p{
	clear:left;
}

.google img{
border: 1px solid cornflowerBlue;
}

.google:hover img{
border: 1px solid midnightBlue;
}

.imdb img{
border: 1px solid cornflowerBlue;
}

.imdb:hover img{
border: 1px solid midnightBlue;
}

.moviemeterimg{
border: 1px solid cornflowerBlue;
}

.moviemeter:hover img{
border: 1px solid midnightBlue;
}

.youtube img{
border: 1px solid cornflowerBlue;
}

.youtube:hover img{
border: 1px solid midnightBlue;
}
		
</style>
<script type="text/javascript">
  var orig_width = 0, orig_height = 0, max_size = 150;

  function resizeme(i)
  {
    orig_width  = i.width;
    orig_height = i.height;
    max_size2   = (max_size > screen.width ? screen.width-75 : max_size);

    if (i.width > i.height)
    {
        if (i.width > max_size2 && orig_width)
        {
            i.width  = max_size2;
            i.height = orig_height * max_size2 / orig_width;
        }
    }
    else
    {
        if (i.height > max_size2 && orig_height)
        {
            i.height = max_size2;
            i.width  = orig_width * max_size2 / orig_height;
        }
    }
  }

  function supersizeme(i)
  {
    if (i.width != orig_width)
    {
        i.width  = orig_width;
        i.height = orig_height;
    }
  }

function maak_nzb_links()
{
  var nzbsite = [
    "http://www.binsearch.info/?q=",
    "http://www.nzb.cc/#",
    "http://www.nzbindex.com/search/?q=",
    "https://www.mysterbin.com/search?q="
  ];
  var zoeknaar = "{firstfile}";

  if (!zoeknaar)
    return;

  var pos = zoeknaar.lastIndexOf('.');
  if (pos != -1)  {
    zoeknaar = zoeknaar.substr(0, pos);
    if (zoeknaar.toLowerCase().indexOf(".part") != -1 || zoeknaar.toLowerCase().indexOf(".vol") != -1) {
      zoeknaar = zoeknaar.substr(0, zoeknaar.lastIndexOf('.'));
    }
  }
  var group = "{newsgroup}".split(" ")[0];
  var extra = "&amp;adv_sort=date&amp;adv_g="+group;

  zoeknaar    = escape(zoeknaar);
  var standaardsite = nzbsite[0]+zoeknaar+extra;

  document.write('<li><a href="'+standaardsite+'" target="_self" >&nbsp;</a>');
  document.write('<ul>');
  document.write('<li><a href="'+standaardsite+'">Binsearch</a></li>');
  document.write('<li><a href="'+nzbsite[1]+zoeknaar+'">Nzb.cc</a><li>');
  document.write('<li><a href="'+nzbsite[2]+zoeknaar+'">NZBIndex</a><li>');
  document.write('<li><a href="'+nzbsite[3]+zoeknaar+'">MysterBin</a><li>');
  document.write('</ul>');
}

function maak_titel_link() 
{
  var zoeksite = [
    "https://encrypted.google.com/search?&q=",
    "http://www.imdb.com/find?s=tt&q=",
    "http://www.moviemeter.nl/film/searchresults/",
    "http://www.youtube.com/results?search_query="
  ];
  var zoeknaar = escape("{title}");
  
  document.write('<a href="'+zoeksite[0]+zoeknaar+'" class="google"><img name="Google" alt="Google" src="b1-google.png" height="41"></a>&nbsp;&nbsp;');
  document.write('<a href="'+zoeksite[1]+zoeknaar+'" class="imdb"><img name="IMDb" alt="IMDb" src="b1-imdb.png" height="41"></a>&nbsp;&nbsp;');
  document.write('<a href="'+zoeksite[2]+zoeknaar+'" class="moviemeter"><img name="Moviemeter" alt="Moviemeter" src="b1-moviemeter.png" height="41"></a>&nbsp;&nbsp;');
  document.write('<a href="'+zoeksite[3]+zoeknaar+'" class="youtube"><img name="YouTube" alt="YouTube" src="b1-youtube.png" height="41"></a>&nbsp;&nbsp;');
}

  
var comments = 0;
</script>

</head>

<body>

<table id="forumoptionstable" style="visibility: hidden; position: absolute; z-index: 5; width: 200px; height: 80px; background-color: #FFFFDD; border: 1px solid #000088; font-family: arial; font-size: 11px;">
<tr><td><a href="#setforumfilter0"><li>Bekijk alle berichten</li></td></tr>
<tr style="background-color: #FFFFCC;"><td><a href="#setforumfilter1"><li>Bekijk alleen nieuwe berichten</li></td></tr>
<tr><td><a href="#setforumfilter2"><li>Bekijk alleen berichten van de spotter</li></td></tr>
<tr><td><a href="#setforumfilter3"><li>Bekijk alleen eigen berichten</li></td></tr>
</table>

<table id="forumoptionstableshadow" style="visibility: hidden; position: absolute; z-index: 4; width: 200px; height: 80px; background-color: #000000; filter: alpha(opacity=30);"><tr><td>&nbsp;</td></tr></table>

<div id="popupbox" style="position: absolute; z-index: 2;"></div>
<div id="popupshadow" style="position: absolute; z-index: 1; filter:alpha(opacity=15);"></div>
<div onMousedown = "" id="closepopup" style="cursor: hand; visibility: hidden; position: absolute; z-index: 3;"><img src="smallcross.gif"></div>


<table style="width:100%;" cellspacing=0 cellpadding=0 border=0>
<tr><td>

<table class="main" border=0 cellspacing=0 cellpadding=0>
<col width=20><col width=23><col width=* align=left><col width=22 align=right><col width=22 align=right><col width=22 align=right><col width=28 align=right><col width=26 align=right><col width=33>

<td background="headerbg.png" ><z id=meldspot><a href="action:reportspot"><img src="meld.png" align="left" onMouseOver="this.src = 'meld01.png'" onMouseOut="this.src = 'meld.png'" border=0></a></z></td>
<td background="headerbg.png" ><img src="gray.png" align="left"></td>
<td class="header"> <span style="height: 16px; overflow: hidden;">{title}</span></td>
<td background="headerbg.png">&nbsp;</td>
<td background="headerbg.png" ><div class="nzb">
<ul>
<script>maak_nzb_links();</script>
</li>
</ul>
</div></td>
<td><img src="subcat/cbg{subcat}.png"></td>
<td><a href="{websiteurl}"><img alt="Link naar het web" src="b1-2.png" onMouseOver="this.src = 'b1-3.png'" onMouseOut="this.src = 'b1-2.png'" border=0></a></td>
<td background="b1-1.png"><div class="search">
<ul>
<li><a href="#" target="_self" >&nbsp;</a>
<ul>
<li><script>maak_titel_link() ;</script></li>
</ul>
</li>
</ul>
</div></td>

<td><img src="headerend.png"></td>
</tr>
</table>

<table class="main" border=0 cellspacing=0 cellpadding=0>
<col width=23><col width=400><col width=*><col width=33>

<tr class="shtext">
<td style="padding-left: 0px;"><img src="subheaderleft.png"><td class="subheader">
<table style="color: #204F7F; width: 560; font-size: 10px; padding-top: 3px;" border=0 cellspacing=0 cellpadding=0>
<col width=168 style="font-weight: bold; padding-left: 3px;"><col width=134><col width=90><col width=70><col width=70>
<tr>
 <td class="subheader_text"><img src="mailicon.gif" border=0><span style="padding-left: 3px;">{spotterlink}</span></td>
              <td class="subheader_text">{date} ({time})</td>
              <td class="subheader_text">{spotid}</td>
              <td class="subheader_text">9976</td>
              <td class="subheader_text">10000</td>
</table>
</td>
<td class="sublength"><span style="white-space: nowrap;"><script>document.write("{newsgroup}".split(" ")[0]);</script></span></td>
<td>
<img src="subheaderend.png"></td></table>
</td><tr><td>
<table style="width: 100%; border-right: 0px solid #409EFF;" cellspacing=0 cellpadding=0 border=0>
<col width=26 style="background-image: url(bleft.png); background-repeat: repeat-y;">
<col width=* style="background-image: url(mainbg.png);">


<tr><td>&nbsp;</td><td style="padding-right: 15px;"><table class="infobox" style="font-size: 11px;">
<tr><td  style="padding: 10px;">

{description}

<P>
<table border=0 cellspacing=0 cellpadding=1 style="width: 100%; font-family: arial; font-size: 11px;"><col style="font-weight: bold; width: 100px; padding-left: 10px;"><col width=10>
<tr><td colspan=3 style="padding-left: 0px;"><i><b><u><div style="font-size: 13px; padding-bottom: 5px;">Spotinformatie</div></u></b></i></td></tr>
 <tr valign=top><td>Categorie&euml;n</td><td>:</td><td>{categories}</td></tr>
 <tr valign=top><td>Weblink</td><td>:</td><td>{website}</td></tr>
 <tr valign=top><td>Tag</td><td>:</td><td>{tag} {taglink}</td></tr>
 <tr valign=top><td>Aantal bestanden</td><td>:</td><td>{numfiles} bestanden</td></tr>
 <tr valign=top><td>Grootte</td><td>:</td><td>{size}</td></tr>
 <tr valign=top><td>Bestandsnaam</td><td>:</td><td><span style="color: #00AA00; font-weight: bold; white-space: nowrap;">{firstfile}</span> t/m <span style="color: #00AA00; font-weight: bold; white-space: nowrap;">{lastfile}</span></td></tr>
 <tr valign=top><td>Nieuwsgroep(en)</td><td>:</td><td>{newsgroup}</td></tr>
</table></td></table>

<tr valign='top' id="commentsbox">
<td >
  <img src="header3.png" />
</td>
<td background="mainbg.png" style="padding-right: 15px; padding-top:3px;">
{comments}

  <table width="100%" cellspacing="0" cellpadding="0" border="0">
  <tr>
    <td width="12">
    <img src="for_l.png" />
    </td>
    <td width="*" background="for_m.png">
    <img src="for_m.png" />
    </td>
    <td width="8">
    <img src="for_r.png" />
    </td>
  </tr>
  </table>
</td>
</tr>

<script>
if (!comments)
{
  comments = 1;
  document.getElementById('commentsbox').style.display = 'none';
}
</script>

<tr><td><a name="forum">&nbsp;</a></td><td align=center style="padding-top: 0px; padding-right: 25px;"><table><tr>
<td><a href="#forumpost">&nbsp;</td>
<td><a href="#kudos">&nbsp;</td>
</table>
</td>
</table><table style="width:100%;" cellspacing=0 cellpadding=0><col width=25><col width=*><tr><td><img src="botleft.png"></td><td align=center style="background-image: url(botbot.png);">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td></table>
<div class="footer"></div></body></html>
