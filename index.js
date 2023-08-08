var binary = require('@mapbox/node-pre-gyp');
var path = require('path');
var binding_path = binary.find(path.resolve(path.join(__dirname,'./package.json')));
var NodeQuickfix = require(binding_path);

exports.logonProvider = NodeQuickfix.FixLoginProvider;
exports.initiator = NodeQuickfix.FixInitiator;
exports.acceptor = NodeQuickfix.FixAcceptor;