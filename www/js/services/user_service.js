app.service('UserService', function ($q) {

  var UserService = function () {
    self = this;
    this.user = null;
  };

  // Logs user in given the scanned info from an rCard.
  // Returns a promise that resolves when login is complete.
  // If this is the first login, the promise will resolve with {firstLogin: true}
  // The app should then give notice to the user that the device is associated with the
  // user.
  UserService.prototype.loginWithRCard = function(str) {
    // Simulates a login. Resolves the promise if SUCCEED is true, rejects if false.
    var SUCCEED = true;

    return $q(function(resolve, reject) {
      setTimeout(function() {
        if (SUCCEED) {
          if (window.localStorage.getItem('notfirstlogin'));
            resolve();
          else
            resolve({firstLogin: true});
          window.localStorage.setItem('notfirstlogin', true);
        } else {
          reject('Login failed.');
        }
      }, 1000);
    });
  };

  // Logs the user out on the remote server.
  // Returns a promise that resolves when logout is complete, or rejects with error of fail.
  UserService.prototype.logout = function() {
    // Simulates logout. Resolves the promise if SUCCEED is true, rejects if false.
    var SUCCEED = true;

    return $q(function(resolve, reject) {
      setTimeout(function() {
        if (SUCCEED) {
          resolve();
        } else {
          reject('Logout failed.');
        }
      }, 1000);
    });
  }

  return new UserService();
});
