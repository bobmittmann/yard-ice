// Modal Dialog Box
// copyright 8th July 2006, 16th April 2011 by Stephen Chapman
// http://javascript.about.com/
// permission to use this Javascript on your web page is granted
// provided that all of the code in this script (including these
// comments) is used without any alteration
function pageWidth() {
	return window.innerWidth != null? window.innerWidth: document.documentElement && document.documentElement.clientWidth ? document.documentElement.clientWidth:document.body != null? document.body.clientWidth:null;
}

function pageHeight() {
	return window.innerHeight != null? window.innerHeight: document.documentElement && document.documentElement.clientHeight ? document.documentElement.clientHeight:document.body != null? document.body.clientHeight:null;
}

function posLeft() {
	return typeof window.pageXOffset != 'undefined' ? window.pageXOffset:document.documentElement && document.documentElement.scrollLeft? document.documentElement.scrollLeft:document.body.scrollLeft? document.body.scrollLeft:0;
}

function posTop() {
	return typeof window.pageYOffset != 'undefined' ? window.pageYOffset:document.documentElement && document.documentElement.scrollTop? document.documentElement.scrollTop: document.body.scrollTop?document.body.scrollTop:0;
}

function _gebi(x){
	return document.getElementById(x);
}

function scrollFix()
{
	var obol=_gebi('ol');
	obol.style.top=posTop()+'px';
	obol.style.left=posLeft()+'px'
}

function sizeFix()
{
	var obol=_gebi('ol');
	obol.style.height=pageHeight()+'px';
	obol.style.width=pageWidth()+'px';
}

function kp(e)
{
	ky=e?e.which:event.keyCode;
	if (ky==88||ky==120) 
		dlg_hide();
	return false
}

function inf(h)
{
	tag=document.getElementsByTagName('select');
	for(i=tag.length-1; i>=0; i--)
		tag[i].style.visibility=h;
	tag=document.getElementsByTagName('iframe');
	for(i=tag.length-1; i>=0; i--)
		tag[i].style.visibility=h;
	tag=document.getElementsByTagName('object');
	for(i=tag.length-1; i>=0; i--)
		tag[i].style.visibility=h;
	tag=document.getElementsByTagName('input');
	for(i=tag.length-1; i>=0; i--)
		tag[i].style.visibility=h;
	tag=document.getElementsByTagName('textarea');
	for(i=tag.length-1; i>=0; i--)
		tag[i].style.visibility=h;
	if ('hidden' === h) {
		tag=_gebi('mbox').getElementsByTagName('input');
		for(i=tag.length-1; i>=0; i--)
			tag[i].style.visibility='visible';
		tag=_gebi('mbox').getElementsByTagName('textarea');
		for(i=tag.length-1; i>=0; i--)
			tag[i].style.visibility='visible';
		tag=_gebi('mbox').getElementsByTagName('select');
		for(i=tag.length-1; i>=0; i--)
			tag[i].style.visibility='visible';
	}
}

var dlg = {
	init : function () {
		var ab='absolute';
		var n='none';
		var obody=document.getElementsByTagName('body')[0];
		var frag=document.createDocumentFragment();
		var obol=document.createElement('div');
		obol.setAttribute('id','ol');
		obol.style.display=n;
		obol.style.position=ab;
		obol.style.top=0;
		obol.style.left=0;
		obol.style.zIndex=998;
		obol.style.width='100%';
		frag.appendChild(obol);
		var obbx=document.createElement('div');
		obbx.setAttribute('id','mbox');
		obbx.style.display=n;
		obbx.style.position=ab;
		obbx.style.zIndex=999;
		var obl=document.createElement('span');
		obbx.appendChild(obl);
		var obbxd=document.createElement('form');
		obbxd.setAttribute('id','dlg');
		obl.appendChild(obbxd);
		frag.insertBefore(obbx,obol.nextSibling);
		obody.insertBefore(frag,obody.firstChild);

		window.onscroll = scrollFix;
		window.onresize = sizeFix;
	},

	getFormElements : function () {
		return document.forms.dlg.elements;
	},
					  
	alert : function (msg) {
		var obbxd=_gebi('dlg');
		var html = '<center><img style="float:none;vertical-align:middle" ' +
				'src="alert.png" alt="Alert! "/>&nbsp;'+msg+'</center>';
		try { obbxd.children['msg'].innerHTML=html; } catch (e) {}
	},

	show : function (obl, txt, wd, ht) {
		var h='hidden';
		var b='block';
		var p='px';
		var obol=_gebi('ol');
		var obbxd=_gebi('dlg');
		obbxd.innerHTML=_gebi(obl).innerHTML;
	//	obbxd.className=_gebi(obl).className;
		try { obbxd.children['title'].innerHTML=txt; } catch (e) {}
		obol.style.height=pageHeight()+p;
		obol.style.width=pageWidth()+p;
		obol.style.top=posTop()+p;
		obol.style.left=posLeft()+p;
		obol.style.display=b;
		var tp=posTop()+((pageHeight()-ht)/2)-12;
		var lt=posLeft()+((pageWidth()-wd)/2)-12;
		var obbx=_gebi('mbox');
		obbx.style.top=(tp<0?0:tp)+p;
		obbx.style.left=(lt<0?0:lt)+p;
		obbx.style.width=wd+p;
	//	obbx.style.height=ht+p;
	//	inf(h);
		obbx.style.display=b;
		return false;
	},

	hide : function() {
		var v='visible';
		var n='none';
		_gebi('ol').style.display=n;
		_gebi('mbox').style.display=n;
	//	inf(v);
		document.onkeypress=''
	}


}

