data.push( { type : 'file', name : 'player.h', content : [






	{ line : 8, type : 'class', name : 'Player', content : {
		public : [
			{ line : 10, type : 'method', name : 'Player', params : [ 'Field::Token', 'std::string' ], content : [
				{ line : 11, type : 'use', member : 'token_' },
				{ line : 12, type : 'use', member : 'name_' }
			]},

			{ line : 15, type : 'method', name : 'Turn', abstraction : 'pure_virtual', return : 'Field::Move', params : [ 'Field' ] },

			{ line : 17, type : 'member', name : 'token_', class : 'Field::Token' },
			{ line : 18, type : 'member', name : 'name_', class : 'std::string' },
		]}
	}
]});
