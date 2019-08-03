var timerId = EventLoop.registerTimer(
  false,
  1000,
  function() {
    console.log("Hola!");
  }
);

var otherTimer = EventLoop.registerTimer(
  true,
  5500,
  function() {
    console.log("Terminating first timer.");
    var result = EventLoop.clearTimer(timerId);
    console.log("result", result);
  }
);

console.log('otherTimer: ', otherTimer);
