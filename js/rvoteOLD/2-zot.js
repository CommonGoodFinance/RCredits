var maintaintop=false,lastsliderchanged="",valueprefs=[],reacting=false,cgsliders=[],maxvalue99=100,maxvalue=100,idealmaxratio=1.2,reactslider=false,reacttext=true,CARPE=CARPE||{};
CARPE.Sliders={version:"2.0 Beta",elements:[],init:function(){var a;CARPE.Sliders.elements=CARPE.getElementsByClass(CARPE.Slider.prototype.sliderClassName);for(var b in CARPE.Sliders.elements){a=CARPE.Slider.prototype.panelClassName+CARPE.Slider.prototype.nameValueSeparator+CARPE.Sliders.elements[b].id;document.getElementById(a)||(cgsliders[cgsliders.length]=new CARPE.Slider(CARPE.Sliders.elements[b]))}cgReactText("")}};
CARPE.Slider=function(a,b,d,f){if(a)if(typeof a==="string")var e=a,c=document.getElementById(a)?document.getElementById(a):null;else if(a.nodeType==1)if(a.tagName.toLowerCase()=="input"||a.tagName.toLowerCase()=="select")c=a;a=c?c:document.createElement("input");if(!a.id&&!e){e=0;for(c=this.sliderClassName+this.idSeparator+this.idPrefix+this.idSeparator;document.getElementById(c+e);)e++;a.id=c+e}this.id=a.id||e;this.name=a.name?a.name:this.id;this.className=a.className?a.className.indexOf(this.sliderClassName)==
-1?a.className+" "+this.sliderClassName:a.className:this.sliderClassName;this.active=true;this.unit=this.defaultUnit;this.hasSlit=true;this.orientation=this.defaultOrientation;this.position=this.defaultPosition;this.value=a.value?parseFloat(a.value):this.defaultValue;this.text=this.texts?this.texts[a.selectedIndex]:"";this.from=this.defaultMinValue;this.to=this.defaultmaxvalue;this.stops=this.stops?this.stops:this.defaultStops;this.size=a.style.width?parseInt(a.style.width,10):this.defaultSize;if(b){this.orientation=
b.orientation?b.orientation:this.orientation;this.value=b.value?parseFloat(b.value):this.value;this.position=b.position?parseInt(b.position,10):this.position;this.size=b.size?parseInt(b.size,10):this.size;this.name=b.name?b.name.toString():this.name;this.stops=b.stops?parseInt(b.stops,10):this.stops;if(b.target)this.target=typeof b.target=="string"?document.getElementById(b.target):b.target;this.feedback=!!b.feedback}this.classNames=this.className?this.className.split(" "):[];for(b=0;b<this.classNames.length;b++){e=
this.classNames[b].split(this.nameValueSeparator)[0];c=this.classNames[b].substring(e.length+1,this.classNames[b].length);switch(e){case "orientation":this.orientation=c;break;case "position":this.position=parseInt(c,10);break;case "value":this.value=parseFloat(c);break;case "size":this.size=parseInt(c,10);break;case "unit":this.unit=c=="px"||c=="mm"||c=="em"?c:this.unit;break;case "slit":this.hasSlit=c=="false"||c=="no"?false:this.hasSlit;break;case "stops":this.stops=parseInt(c,10);break;case "from":this.from=
parseInt(c,10);break;case "to":this.to=parseInt(c,10);break;case "target":this.target=document.getElementById(c)?document.getElementById(c):this.target;break;case "feedback":this.feedback=c==="true"||c==="yes"?true:!!c;break;default:break}}this.xMax=this.orientation==this.xOrientation?this.size:0;this.yMax=this.orientation==this.yOrientation?this.size:0;this.value=this.value>=maxvalue?maxvalue:this.value<this.from?this.from:this.value;this.position=this.position>this.size?this.size:this.position;
if(this.value===this.defaultValue)this.value=this.size>0?this.position/this.size*(maxvalue-this.from)+this.from:maxvalue;else this.position=this.value<=maxvalue?Math.round((this.value-this.from)/(maxvalue-this.from)*this.size):this.size;this.x=this.y=this.position;if(a.parentNode)this.parent=a.parentNode;else{this.parent=CARPE.defaultParentNode;if(d){if(d.parent)this.parent=typeof d.parent==="string"?document.getElementById(d.parent):d.parent;if(d.before){this.before=typeof d.before==="string"?document.getElementById(d.before):
d.before;this.parent=this.before.parentNode}if(d.after){this.after=typeof d.after==="string"?document.getElementById(d.after):d.after;this.parent=this.after.parentNode}}if(!this.before&&!this.after)this.parent.appendChild(a);else if(this.before)this.parent.insertBefore(a,this.before);else if(this.after){for(d=this.after.nextSibling;d.nodeType!=1;)d=d.nextSibling;this.parent.insertBefore(a,d)}}this.valueElmnt=document.createElement("input");this.valueElmnt.setAttribute("type","hidden");this.valueElmnt.setAttribute("name",
this.name);this.valueElmnt.className=this.className;this.parent.insertBefore(this.valueElmnt,a);this.parent.removeChild(a);this.valueElmnt.id=this.id;this.valueElmnt.knob=this.knob;this.valueElmnt.panel=this.panel;this.valueElmnt.slit=this.slit;this.valueElmnt.setValue=this.setValue.bind(this);this.panel=document.createElement("a");this.panel.setAttribute("href","javascript: void 0;");this.panel.style.cssText=a.style.cssText;this.parent.insertBefore(this.panel,this.valueElmnt);this.panel.className=
this.panelClassName+" orientation-"+this.orientation;this.panel.id=this.panelClassName+"-"+this.id;this.knob=document.createElement("div");this.knob.className=this.knobClassName;this.knob.id=this.knobClassName+"-"+this.id;for(this.panel.appendChild(this.knob);!this.knob.width;){this.knob.width=parseInt(CARPE.getStyle(this.knob,"width"),10);window.setTimeout("",100)}this.knob.height=parseInt(CARPE.getStyle(this.knob,"height"),10);if(this.orientation==this.xOrientation){d=this.size+this.knob.width;
window.opera||(d+=parseInt(CARPE.getStyle(this.knob,"border-left-width"),10)+parseInt(CARPE.getStyle(this.knob,"border-right-width"),10));this.panel.style.width=d+this.unit}else{d=this.size+this.knob.height;window.opera||(d+=parseInt(CARPE.getStyle(this.knob,"border-top-width"),10)+parseInt(CARPE.getStyle(this.knob,"border-bottom-width"),10));this.panel.style.height=d+this.unit}if(this.hasSlit){this.slit=document.createElement("div");this.slit.className=this.slitClassName;this.slit.id=this.slitClassName+
"-"+this.id;this.panel.appendChild(this.slit);if(this.orientation==this.xOrientation){this.slit.style.width=this.size+this.knob.width-parseInt(CARPE.getStyle(this.slit,"border-left-width"),10)-parseInt(CARPE.getStyle(this.slit,"border-right-width"),10)+this.unit;if(window.opera)this.slit.style.width=parseInt(this.slit.style.width,10)-parseInt(CARPE.getStyle(this.knob,"border-left-width"),10)-parseInt(CARPE.getStyle(this.knob,"border-right-width"),10)+this.unit}else{this.slit.style.height=this.size+
this.knob.height-parseInt(CARPE.getStyle(this.slit,"border-top-width"),10)-parseInt(CARPE.getStyle(this.slit,"border-bottom-width"),10)+this.unit;if(window.opera)this.slit.style.height=parseInt(this.slit.style.height,10)-parseInt(CARPE.getStyle(this.knob,"border-top-width"),10)-parseInt(CARPE.getStyle(this.knob,"border-bottom-width"),10)+this.unit}}if(f)for(var g in f)if(g=="panel"||g=="knob"||g=="slit")for(var h in f[g])this[g].style[h]=f[g][h];CARPE.addEventListener(this.knob,"mousedown",this.slide.bind(this));
CARPE.addEventListener(this.panel,"mousedown",this.slideTo.bind(this));this.panel.onblur=this.makeBlurred.bind(this);if(window.opera)this.panel.onkeypress=this.keyHandler.bind(this);else this.panel.onkeydown=this.keyHandler.bind(this);this.update()};
CARPE.Slider.prototype={defaultParentNode:document.forms[0]?document.forms[0]:document,defaultUnit:"px",defaultPosition:0,defaultValue:null,defaultMinValue:0,defaultmaxvalue:1,defaultSize:100,defaultStops:0,xOrientation:"horizontal",yOrientation:"vertical",defaultOrientation:"horizontal",nameValueSeparator:"-",idPrefix:"auto",idSeparator:"-",sliderClassName:"carpe-slider",panelClassName:"carpe-slider-panel",slitClassName:"carpe-slider-slit",knobClassName:"carpe-slider-knob",makeFocused:function(){},
makeBlurred:function(){if(this.hasSlit)this.slit.className=this.slitClassName;return this},keyHandler:function(a){if(a=a||window.event){a=a.which||a.keyCode;if(a==CARPE.KEY_RIGHT||a==CARPE.KEY_UP){this.orientation==this.xOrientation?this.moveInc(1):this.moveInc(-1);return false}else if(a==CARPE.KEY_LEFT||a==CARPE.KEY_DOWN){this.orientation==this.xOrientation?this.moveInc(-1):this.moveInc(1);return false}}return true},moveInc:function(a){return this.stops>1?this.moveToPos(this.position+this.size/(this.stops-
1)*a):this.moveToPos(this.position+a)},mouseUp:function(){this.sliding=false;this.stops>1?this.moveToPos(parseInt(this.size*Math.round(this.position*(this.stops-1)/this.size)/(this.stops-1),10)):this.updateTarget(this.feedback);cgReactText("");CARPE.removeEventListener(document,"mousemove",this.mouseMoveListener);CARPE.removeEventListener(document,"mouseup",this.mouseUpListener);if(this.hasSlit)this.slit.className=this.slitClassName;this.panel.focus();return this},slide:function(a){a=a||window.evnt;
CARPE.stopEvent(a);this.panel.focus();this.startOffsetX=this.x-a.screenX;this.startOffsetY=this.y-a.screenY;this.sliding=true;this.mouseMoveListener=this.moveSlider.bind(this);CARPE.addEventListener(document,"mousemove",this.mouseMoveListener);this.mouseUpListener=this.mouseUp.bind(this);CARPE.addEventListener(document,"mouseup",this.mouseUpListener);return true},slideTo:function(a){a=a||window.event;CARPE.stopEvent(a);this.orientation===this.xOrientation?this.moveToPos(a.clientX-(CARPE.getPos(this.knob).x-
this.x+parseInt(this.knob.width/2,10))):this.moveToPos(a.clientY-(CARPE.getPos(this.knob).y-this.y+parseInt(this.knob.height/2,10)));this.slide(a);return true},moveSlider:function(a){a=a||window.event;if(this.sliding){this.orientation==this.xOrientation?this.moveToPos(this.startOffsetX+a.screenX,true):this.moveToPos(this.startOffsetY+a.screenY);return false}},moveToPos:function(a,b){if(this.orientation==this.xOrientation){this.x=a>this.xMax?this.xMax:a<0?0:a;this.value=this.x/this.size*(maxvalue-
this.from)+this.from;this.position=CARPE.left(this.knob.id,this.x)}else{this.y=a>this.yMax?this.yMax:a<0?0:a;this.value=1-(this.y/this.size*(maxvalue-this.from)+this.from);this.position=CARPE.top(this.knob.id,this.y)}reactslider&&this.cgReact();if(reacttext)b||cgReactText("");this.text=this.texts?this.texts[Math.round(this.value*(this.texts.length-1))]:"";return this},setValue:function(a,b){a=a>maxvalue?maxvalue:a;a=a<this.from?this.from:a;return this.moveToPos(Math.round(a*this.size/(maxvalue-this.from),
10),b)},updateTarget:function(){if(this.target)if(this.target.setValue)this.target.setValue(this.valueElmnt.value);else if(document.getElementById(this.target.id).setValue){this.target=document.getElementById(this.target.id);this.target.setValue(this.valueElmnt.value);return this.updateTarget()}else this.target.value=this.valueElmnt.value;return this},update:function(){return this.setValue(this.value||this.target.value,true)},kill:function(){this.setValue("",true);for(i in cgsliders)i.id==this.id&&
this.splice(i,1)}};function cgReactText(a){var b,d=a?parseInt(cgsliders[a].target.value.replace("%","")):0,f=-d,e;for(var c in cgsliders)if(cgsliders[c].active)f+=cgsliders[c].value;for(c in cgsliders){b=cgsliders[c];if(b.active&&c!=a){e=f==0?0:b.value*(100-d)/f;b.valueElmnt.value=e.toFixed(1)+"%";b.updateTarget()}}}function pr(a){var b=document.getElementById("testoutput");b.innerHTML=b.innerHTML+"<br>"+a};