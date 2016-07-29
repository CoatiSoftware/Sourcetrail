KIT.FileViewer = {

	view : null,

	lastFile : null,
	lastLines : [],

	init : function( view ) {
		this.view = view;

		SyntaxHighlighter.defaults['tab-size'] = 2;
		SyntaxHighlighter.defaults['toolbar'] = false;
		SyntaxHighlighter.all();
	},

	showNavigation : function( data ) {
		var name;
		var that = this;
		for ( var i = data.length - 1; i >= 0; i-- ) {
			var fileName = data[i].name;
			name = fileName.split( '.' )[0];
			$( '<div class="button" id="' + name + '_button">' ).text( fileName ).appendTo( '#header' ).click( function( name ) {
				return function() {
					KIT.FileViewer.showFile( name );
				}
			}( name ));

			if ( i === 0 ) {
				this.showFile( name );
			}

			$( '#' + name + ' .syntaxhighlighter .code .line' ).click( function( name ) {
				return function() {
					var lineNumber = parseInt( $( this ).attr( 'class' ).substring( 11 ) );
					that.view.activateLocation( new KIT.Location( name, lineNumber ) );
				}
			}( name ));
		}
	},

	resetLast : function() {
		if ( this.lastLines.length ) {
			for ( var i = 0; i < this.lastLines.length; i++ ) {
				$( '#' + this.lastFile + ' .code .number' + this.lastLines[i] ).removeClass( 'highlighted' );
			}
			this.lastLines = [];
		}
	},

	showFile : function( file ) {
		this.resetLast();

		if ( this.lastFile === file ) {
			return;
		}

		if ( this.lastFile ) {
			$( '#' + this.lastFile ).hide();
			$( '#' + this.lastFile + '_button' ).removeClass( 'selected' );
		}

		$( '#' + file ).show();
		$( '#' + file + '_button' ).toggleClass( 'selected' );
		this.lastFile = file;
	},

	addLocation : function( location ) {
		if ( this.lastFile !== location.fileName ) {
			this.showFile( location.fileName );
		}
		this.showLine( location.lineNumber );
	},

	addLocations : function( locations ) {
		for ( var i = 0; i < locations.length; i++ ) {
			if ( this.lastFile !== locations[i].fileName ) {
				this.showFile( locations[i].fileName );
			}
			this.showLine( locations[i].lineNumber );
		}
	},

	showLocation : function( location ) {
		this.showFile( location.fileName );
		this.showLine( location.lineNumber );
		this.scrollToLine();
	},

	showLocations : function( locations ) {
		this.showFile( locations[0].fileName );
		for ( var i = 0; i < locations.length; i++ ) {
			this.showLine( locations[i].lineNumber );
		}
		this.scrollToLine();
	},

	showLine : function( line ) {
		$( '#' + this.lastFile + ' .code .number' + line ).addClass( 'highlighted' );
		this.lastLines.push( line );
	},

	scrollToLine : function() {
		if ( this.lastLines.length ) {
			$( '#wrapper' ).scrollTo( '#' + this.lastFile + ' .code .number' + this.lastLines[0], { offset : -200 } );
		}
	}
};