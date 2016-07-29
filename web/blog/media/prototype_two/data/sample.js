data.push( { type : 'file', name : 'sample.cpp', content : [
	{ line : 1, type : 'class', name : 'Player', content : {
		public : [
			{ line : 3, type : 'method', name : 'Do', return : 'void' }
		]
	}},
	{ line : 6, type : 'class', name : 'Base' },

	{ line : 8, type : 'class', name : 'Game', base : 'Base', content : {
		public : [
			{ line : 10, type : 'method', name : 'Game', content : [
				{ line : 11, type : 'call', method : 'Init' }
			]},

			{ line : 14, type : 'method', name : 'Init' },

			{ line : 16, type : 'method', name : 'Run', content : [
				{ line : 17, type : 'use', member : 'player' }, { line : 17, type : 'call', method : 'Do', class : 'Player' }
			]}
		],
		private : [
			{ line : 21, type : 'member', name : 'player', class : 'Player' }
		]
	}},
	{ line : 24, type : 'variable', name : 'game', class : 'Game' },

	{ line : 26, type : 'function', name : 'main', return : 'int', content : [
		{ line : 27, type : 'use', variable : 'game' }, { line : 27, type : 'call', method : 'Game', class : 'Game' },

		{ line : 29, type : 'use', variable : 'game' }, { line : 29, type : 'call', method : 'Run', class : 'Game' },

		{ line : 31, type : 'use', variable : 'game' }
	]}
]});
