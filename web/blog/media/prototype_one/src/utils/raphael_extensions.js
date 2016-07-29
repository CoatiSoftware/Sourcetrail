extend( Raphael.el, {
	box : function() {
		return this.attr({
			fill : 'white',
			stroke : '#555',
			'stroke-width' : 1.5
		});
	},

	line : function( color, width ) {
		return this.attr({
			'stroke-width' : width || 1.5,
			stroke : color
		});
	},

	thick : function() {
		return this.attr({
			'stroke-width' : 3
		});
	},

	thin : function() {
		return this.attr({
			'stroke-width' : 1
		});
	},

	dash : function() {
		return this.attr({
			'stroke-dasharray' : ['- ']
		});
	},

	code : function( size ) {
		return this.attr({
			font : size + 'px Consolas, monospace',
			fill : '#555'
		});
	},

	stroke : function( color ) {
		this.attr({
			stroke : 'rgba(0,0,0,1)'
		});
		return this.attr({
			stroke : color
		});
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
	}
});

Raphael.st.add = function( name ) {
	this[name] = function() {
		var args = arguments;
		return this.forEach( function( el ) {
			el[name].apply( el, args );
		});
	};
}

Raphael.st.add( 'stroke' );
Raphael.st.add( 'fill' );
Raphael.st.add( 'color' );
Raphael.st.add( 'dash' );
Raphael.st.add( 'thin' );
Raphael.st.add( 'thick' );
Raphael.st.add( 'line' );

extend( Raphael.fn, {
	getPoints : function( obj ) {
		var bb = obj.getBBox();
		return [
			{x : bb.x + bb.width / 2, y : bb.y - 1, i : 0},
			{x : bb.x2 + 1, y : bb.y + bb.height / 2, i : 1},
			{x : bb.x + bb.width / 2, y : bb.y2 + 1, i : 2},
			{x : bb.x - 1, y : bb.y + bb.height / 2, i : 3}
		];
	},

	connectionPoints : function( obj1, obj2 ) {
		var p1 = this.getPoints( obj1 );
		var p2 = this.getPoints( obj2 );

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

	shortestPoints : function( obj1, obj2 ) {
		var bb1 = obj1.getBBox();
		var bb2 = obj2.getBBox();

		var p1 = { x : 0, y : 0, i : 0 };
		var p2 = { x : 0, y : 0, i : 0 };

		var x1 = Math.max( bb1.x, bb2.x );
		var x2 = Math.min( bb1.x2, bb2.x2 );

		if ( x1 < x2 ) {
			p1.x = p2.x = ( x1 + x2 ) / 2;
		} else if ( bb1.x2 === x2 ) {
			p1.x = x2;
			p2.x = x1;
		} else {
			p1.x = x1;
			p2.x = x2;
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

		return [p1, p2];
	},

	wave : function( obj1, obj2 ) {
		var p = this.connectionPoints( obj1, obj2 );
		var x2, y2, x3, y3;
		if ( p[0].i % 2 == 0 ) {
			x2 = p[0].x;
			y2 = ( p[0].y + p[1].y ) / 2;
		} else {
			x2 = ( p[0].x + p[1].x ) / 2;
			y2 = p[0].y;
		}
		if ( p[1].i % 2 == 0 ) {
			x3 = p[1].x;
			y3 = ( p[0].y + p[1].y ) / 2;
		} else {
			x3 = ( p[0].x + p[1].x ) / 2;
			y3 = p[1].y;
		}
		var path = this.path( ['M', p[0].x, p[0].y, 'C', x2, y2, x3, y3, p[1].x, p[1].y].join( ' ' ) );
		path.points = p;
		return path;
	},

	bow : function( obj1, obj2, f ) {
		var p1 = this.getPoints( obj1 );
		var p2 = this.getPoints( obj2 );
		if ( p1[1].y < p2[1].y ) {
			p1 = p1[3];
			p2 = p2[3];
			f *= -1;
		} else {
			p1 = p1[1];
			p2 = p2[1];
		}
		var mid = new Vector().copy( p1 ).addSelf( p2 ).divSelf( 2 );
		mid.x += clamp( f * Math.abs( p1.y - p2.y ), -200, 200 );
		var bow = this.path( ['M', p1.x, p1.y, 'S', mid.x, mid.y, p2.x, p2.y].join( ' ' ) );
		bow.points = [p1, p2];
		return bow;
	},

	arrow : function( x, y, i ) {
		arrow = [
			['l', -5, -10, 'm', 10, 0, 'l', -5, 10],
			['l', 10, 5, 'm', 0, -10, 'l', -10, 5],
			['l', 5, 10, 'm', -10, 0, 'l', 5, -10],
			['l', -10, -5, 'm', 0, 10, 'l', 10, -5]
		];
		return this.path( ['M', x, y].concat( arrow[i] ).join( ' ' ) );
	},

	arrow2 : function( obj1, obj2 ) {
		var p = this.connectionPoints( obj2, obj1 );
		var vec = new Vector().copy( p[1] ).subSelf( p[0] );
		var l = vec.norm();
		var r = vec.angle() / Math.PI * 180;
		var x = 20;
		var y = 10;
		var path = this.path( ['M', 0, 0, 'l', l - x, 0, 0, -y, x, y, -x, y, 0, -y].join( ' ' ) );
		path.transform( ['R', r, 0, 0, 'T', p[0].x, p[0].y].join( ' ' ) );
		return path;
	},

	connection : function( obj1, obj2 ) {
		var p = this.connectionPoints( obj1, obj2 );
		return this.path( ['M', p[1].x, p[1].y, 'L', p[0].x, p[0].y].join( ' ' ) );
	},

	shortest : function( obj1, obj2 ) {
		var p = this.shortestPoints( obj1, obj2 );
		return this.path( ['M', p[1].x, p[1].y, 'L', p[0].x, p[0].y].join( ' ' ) );
	},

	shortest2 : function( obj1, obj2 ) {
		var p = this.shortestPoints( obj1, obj2 );
		p[1] = this.shortestTo( p[0], this.getPoints( obj2 ) );
		return this.path( ['M', p[1].x, p[1].y, 'L', p[0].x, p[0].y].join( ' ' ) );
	},

	checkbox : function( x, y, size, checked ) {
		var s = size;
		x -= Math.floor( s / 2 );
		y -= Math.floor( s / 2 );

		var rect = this.rect( x, y, s, s ).attr({ fill : 'rgba(0,0,0,0)' });
		var path = this.path( ['M', x, y, 'l', s, s, 'm', -s, 0, 'l', s, -s].join( ' ' ) );
		if ( !checked ) {
			path.hide();
		}

		var handler = null;
		var set = this.set().push(rect, path);
		set.line( SHIFT.Colors.dark );
		set.click( function() {
			checked = !checked;
			checked ? path.show() : path.hide();
			if ( handler ) {
				handler( checked )
			}
		});

		set.click = function( callback ) {
			handler = callback;
			return this;
		}
		set.hover( function() {
			set.stroke( SHIFT.Colors.light );
		}, function() {
			set.stroke( SHIFT.Colors.dark );
		});
		return set;
	}
})
