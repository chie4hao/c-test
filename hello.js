const addon = require('./build/Release/addon');

addon((msg) => {
  console.log(msg);
  // Prints: 'hello world'
});

addon((msg) => {
  console.log(msg);
  // Prints: 'hello world'
});