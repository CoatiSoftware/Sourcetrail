SHIFT.FileViewer = {

	lastFile : null,
	lastLines : [],

	Init : function() {
		SyntaxHighlighter.defaults['tab-size'] = 2;
		SyntaxHighlighter.defaults['toolbar'] = false;
		SyntaxHighlighter.all();
	},

	ShowNavigation : function() {
		var name;
		for ( var i = 0; i < data.length; i++ ) {
			var fileName = data[i].name;
			name = fileName.split( '.' )[0];
			$( '<div id="' + name + '_button">' ).text( fileName ).appendTo( '#header' ).click( function( name ) {
				return function() {
					SHIFT.FileViewer.ShowFile( name );
				}
			}( name ));

			if ( i === 0 ) {
				this.ShowFile( name );
			}
		}
	},

	ShowFile : function( file ) {
		if ( this.lastLines.length ) {
			for ( var i = 0; i < this.lastLines.length; i++ ) {
				$( '#' + this.lastFile + ' .code .number' + this.lastLines[i] ).removeClass( 'highlighted' );
			}
			this.lastLines = [];
		}

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

	ShowLocation : function( location ) {
		this.ShowFile( location.fileName );
		this.ShowLine( location.lineNumber );
		this.ScrollToLine( location.lineNumber );
	},

	ShowLocations : function( locations ) {
		this.ShowFile( locations[0].fileName );
		for ( var i = 0; i < locations.length; i++ ) {
			this.ShowLine( locations[i].lineNumber );
		}
		this.ScrollToLine( locations[0].lineNumber );
	},

	ShowLine : function( line ) {
		$( '#' + this.lastFile + ' .code .number' + line ).addClass( 'highlighted' );
		this.lastLines.push( line );
	},

	ScrollToLine : function( line ) {
		$( '#files' ).scrollTo( '#' + this.lastFile + ' .code .number' + line, { offset : -200 } );
	}
};