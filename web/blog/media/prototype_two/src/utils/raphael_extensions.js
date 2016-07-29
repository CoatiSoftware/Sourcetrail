var RaphaelElementExtensions = {

	stroke : function( color, width ) {
		if ( width ) {
			this.attr({
				'stroke-width' : width
			});
		}
		this.attr({
			stroke : 'rgba(0,0,0,1)'
		});
		return this.attr({
			stroke : color
		});
	},

	noStroke : function() {
		return this.attr({
			stroke : null
		})
	},

	fill : function( color ) {
		return this.attr({
			fill : color
		});
	},

	color : function( color ) {
		this.stroke( color );
		this.fill( color );
		return this;
	},

	code : function( size, color ) {
		return this.attr({
			font : size + 'px Consolas, monospace',
			// 'text-anchor' : 'start',
			fill : color
		});
	},

	dash : function() {
		return this.attr({
			'stroke-dasharray' : ['- ']
		});
	},

	noDash : function() {
		return this.attr({
			'stroke-dasharray' : []
		})
	},

	round : function() {
		return this.attr({
			'stroke-linecap' : 'round',
			'stroke-linejoin' : 'round'
		});
	}

};

RaphaelFunctionExtensions = {

	line : function( x, y, x2, y2 ) {
		return this.path( ['M', x, y, 'L', x2, y2].join( ' ' ) );
	},

	connection : function( bb1, bb2, endPointType, lineType ) {
		var points;
		var line;

		switch ( endPointType ) {
			case 'middle' :
				points = this.getMidConnectionPoints_( bb1, bb2 );
				break;
			case 'same' :
				points = this.getSameConnectionPoints_( bb1, bb2 );
				break;
			case 'shortest' :
				points = this.getShortestConnectionPoints_( bb1, bb2 );
				break;
			case 'center' :
				points = this.getCenterPoints_( bb1, bb2 );
				break;
			default :
				console.error( 'End point type unknown', endPointType );
				break;
		}

		switch ( lineType ) {
			case 'line' :
				line = this.createLine_( points );
				this.addNormal_( points );
				break;
			case 'wave' :
				line = this.createWave_( points );
				break;
			case 'bow' :
				line = this.createBow_( points );
				break;
			default :
				console.error( 'Line type unknown', lineType );
				break;
		}

		line.start = points[0];
		line.end = points[1];

		return line;
	},

	arrow : function( point, length, width, closed ) {
		var p = point;
		var u = new Vector( point.nx, point.ny ).normalizeSelf();
		var a = u.rotate( TAU / 4 );

		u.mulSelf( length );
		a.mulSelf( width );

		return this.path( [
			'M', p.x, p.y,
			'l', u.x - a.x, u.y - a.y,
			closed ? 'l' : 'm', 2 * a.x, 2 * a.y,
			'L', p.x, p.y].join( ' ' ) );
	},

	getMidPoints_ : function( bb ) {
		return [
			{x : bb.x + bb.width / 2, y : bb.y, nx : 0, ny : -1 },
			{x : bb.x2, y : bb.y + bb.height / 2, nx : 1, ny : 0 },
			{x : bb.x + bb.width / 2, y : bb.y2, nx : 0, ny : 1 },
			{x : bb.x, y : bb.y + bb.height / 2, nx : -1, ny : 0 }
		];
	},

	getMidConnectionPoints_ : function( bb1, bb2 ) {
		var p1 = this.getMidPoints_( bb1 );
		var p2 = this.getMidPoints_( bb2 );

		var vec = new Vector;
		var min = Infinity, a, b;
		for ( var i = 0; i < 4; i++ ) {
			for ( var j = 0; j < 4; j++ ) {
				vec.copy( p1[i] ).subSelf( p2[j] );
				var dist = vec.normSquared();
				if ( dist < min ) {
					min = dist;
					a = p1[i];
					b = p2[j];
				}
			}
		}
		return [a, b];
	},

	getSameConnectionPoints_ : function( bb1, bb2 ) {
		var p1 = this.getMidPoints_( bb1 );
		var p2 = this.getMidPoints_( bb2 );

		var vec = new Vector;
		var min = Infinity, a, b;
		for ( var i = 0; i < 4; i++ ) {
			vec.copy( p1[i] ).subSelf( p2[i] );
			var dist = vec.normSquared();
			if ( dist < min ) {
				min = dist;
				a = p1[i];
				b = p2[i];
			}
		}
		return [a, b];
	},

	getShortestConnectionPoints_ : function( bb1, bb2 ) {
		var p1 = { x : 0, y : 0, nx : 0, ny : 0 };
		var p2 = { x : 0, y : 0, nx : 0, ny : 0 };

		var x1 = Math.max( bb1.x, bb2.x );
		var x2 = Math.min( bb1.x2, bb2.x2 );

		if ( x1 < x2 ) {
			p1.x = p2.x = ( x1 + x2 ) / 2;
			p1.ny = p2.ny = 1;
		} else if ( bb1.x2 === x2 ) {
			p1.x = x2;
			p2.x = x1;
			p1.nx = p2.nx = 1;
		} else {
			p1.x = x1;
			p2.x = x2;
			p1.nx = p2.nx = 1;
		}

		var y1 = Math.max( bb1.y, bb2.y );
		var y2 = Math.min( bb1.y2, bb2.y2 );

		if ( y1 < y2 ) {
			p1.y = p2.y = ( y1 + y2 ) / 2;
		} else if ( bb1.y2 === y2 ) {
			p1.y = y2;
			p2.y = y1;
		} else {
			p1.y = y1;
			p2.y = y2;
		}

		if ( bb1.y < bb2.y ) {
			p2.ny *= -1;
		} else {
			p1.ny *= -1;
		}

		if ( bb1.x < bb2.x ) {
			p2.nx *= -1;
		} else {
			p1.nx *= -1;
		}

		return [p1, p2];
	},

	getCenterPoints_ : function( bb1, bb2 ) {
		return [
			{x : bb1.x + bb1.width / 2, y : bb1.y + bb1.height / 2, nx : 0, ny : 1 },
			{x : bb2.x + bb2.width / 2, y : bb2.y + bb2.height / 2, nx : 0, ny : -1 },
		];
	},

	getNormal_ : function( points ) {
		return new Vector( points[1].x - points[0].x, points[1].y - points[0].y ).normalizeSelf();
	},

	addNormal_ : function( points ) {
		var n = this.getNormal_( points );
		points[0].nx = n.x;
		points[0].ny = n.y;
		points[1].nx = -n.x;
		points[1].ny = -n.y;
	},

	createLine_ : function( points ) {
		return this.path( ['M', points[1].x, points[1].y, 'L', points[0].x, points[0].y].join( ' ' ) );
	},

	createWave_ : function( points ) {
		var x2, y2, x3, y3;
		if ( points[0].nx === 0 ) {
			x2 = points[0].x;
			y2 = ( points[0].y + points[1].y ) / 2;
		} else {
			x2 = ( points[0].x + points[1].x ) / 2;
			y2 = points[0].y;
		}
		if ( points[1].nx === 0 ) {
			x3 = points[1].x;
			y3 = ( points[0].y + points[1].y ) / 2;
		} else {
			x3 = ( points[0].x + points[1].x ) / 2;
			y3 = points[1].y;
		}
		return this.path( ['M', points[0].x, points[0].y, 'C', x2, y2, x3, y3, points[1].x, points[1].y].join( ' ' ) );
	},

	createBow_ : function( points ) {
		var f = 3;
		if ( points[0].nx < 0 ) {
			f *= -1;
		}
		var mid = new Vector().copy( points[0] ).addSelf( points[1] ).divSelf( 2 );
		mid.x += clamp( f * Math.abs( points[0].y - points[1].y ), -200, 200 );
		return this.path( ['M', points[0].x, points[0].y, 'S', mid.x, mid.y, points[1].x, points[1].y].join( ' ' ) );
	},


	shortestTo : function( p, q ) {
		var vec = new Vector;
		var min = Infinity, r;
		for ( var i = 0; i < 4; i++ ) {
			vec.copy( p ).subSelf( q[i] );
			var d = vec.normSquared();
			if ( d < min ) {
				min = d;
				r = q[i];
			}
		}
		return r;
	},

	shortest2 : function( obj1, obj2 ) {
		var p = this.shortestPoints( obj1, obj2 );
		p[1] = this.shortestTo( p[0], this.getPoints( obj2 ) );
		return this.path( ['M', p[1].x, p[1].y, 'L', p[0].x, p[0].y].join( ' ' ) );
	}
};

extend( Raphael.el, RaphaelElementExtensions);
extend( Raphael.fn, RaphaelFunctionExtensions);

function addToSet( name ) {
	Raphael.st[name] = function() {
		var args = arguments;
		return this.forEach( function( el ) {
			el[name].apply( el, args );
		});
	};
};

for (var key in RaphaelElementExtensions) {
	addToSet( key );
}
