(function(app) {
  'use strict';

  app.service('NetworkService', function($rootScope) {

    var self;
    var NetworkService = function() {
      self = this;
      this.connectionOnline = true;
      this.init_();
    };

    NetworkService.prototype.init_ = function() {
      document.addEventListener("online", this.onOnline.bind(this), false);
      document.addEventListener("offline", this.onOffline.bind(this), false);

      if (navigator && navigator.connection) {
        this.connectionOnline = navigator.connection.type !== 'none';
      }
      //console.log("Connection is online: ", !this.isOffline());

      $rootScope.isOffline = function() {
        return self.isOffline();
      }

    };

    NetworkService.prototype.onOffline = function() {
      console.warn("OFFLINE");
      this.connectionOnline = false;
      $rootScope.$apply();
    };

    NetworkService.prototype.onOnline = function() {
      console.info("ONLINE");
      this.connectionOnline = true;
      $rootScope.$apply();
    };

    NetworkService.prototype.isOffline = function() {
      return !this.connectionOnline;
    };


    return new NetworkService();
  });

})(app);
