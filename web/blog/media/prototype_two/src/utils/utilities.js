function bind(scope, fn) {
  
  if ( fn ) {
  
    return function() {
    
      fn.apply(scope, arguments);
    
    };
  
  }
  
};

function extend(destination, source) {
  
  for (var key in source) {
    
    if (source.hasOwnProperty(key)) {
      
      destination[key] = source[key];
      
    }
    
  }
  
  return destination;
  
};

function log() {
  
  console.log.apply(console, arguments);
  
};

function clamp( x, a, b ) {

  return x ? x < a ? a : x > b ? b : x : a;

};

function map( x, a1, a2, b1, b2 ) {

  return ( x  - a1 ) * ( b2 - b1 ) / ( a2 - a1 ) + b1;

};

function checkAngle( angle ) {
  
  if ( angle > Math.PI ) {
    
    angle -= 2 * Math.PI;
    
  } else if ( angle <= -Math.PI ) {
    
    angle += 2 * Math.PI;
    
  }
  
  return angle;
  
};

function rand( min, max ) {
  
  min = min || 0;
  max = max || 1;
  
  return Math.random() * (max - min) + min;
  
};

function randInt( min, max ) {
  
  return Math.floor( rand( min, max ) );
  
};

function randSign() {
  
  return Math.random() > 0.5 ? 1 : -1;
  
};

function randBool() {
  
  return Math.random() > 0.5;
  
};

window.TAU = 2 * Math.PI;

function toRad( deg ) {
  
  return deg * TAU / 360;
  
};

function toDeg( rad ) {
  
  return rad / TAU * 360;
  
};

Array.prototype.forEachApply = function( fn, a ) {

  this.forEach( function( e ) {
    
    e[fn].call( e, a );
    
  });
  
};

Array.prototype.clone = function() {
  
  return this.concat();
  
};

Array.prototype.shuffle = function() {
  
  var i = this.length, j, swap;
  
  while ( i-- > 0 ) {
    
    j = Math.floor( Math.random() * ( i + 1 ) );
    
    swap = this[i];
    this[i] = this[j];
    this[j] = swap;
    
  }
  
  return this;
  
}

Array.max = function( array ) {
  
  return Math.max.apply( Math, array );
  
};

Array.min = function( array ) {
  
  return Math.min.apply( Math, array );
  
};

Array.prototype.unique = function() {
    var a = [];
    for ( var i = 0; i < this.length; i++ ) {
        if ( a.indexOf( this[i] ) === -1 ) {
            a.push( this[i] );
        }
   }
   return a;
};

function getURLParams() {

	var params = {},
		floatRegex = /^[-+]?\d*\.?\d+$/,
		results = window.location.href.match( /[^?&#]*=[^&#]*/g ) || [],
		a, i;

	for ( i = 0; i < results.length; i++ ) {

		a = results[i].split('=');

		params[a[0]] = floatRegex.test( a[1] ) ? parseFloat( a[1] ) : a[1];

	}

	return params;

};
