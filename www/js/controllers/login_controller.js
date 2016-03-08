app.controller('LoginCtrl', function($scope, $state, $ionicLoading, BarcodeService, UserService, $ionicHistory, NotificationService, CashierModeService) {

  // Scanner Login

  $ionicHistory.clearHistory();
  $scope.openScanner = function() {
    $ionicLoading.show();

    BarcodeService.scan()
      .then(function(str) {
        UserService.loginWithRCard(str)
          .then(function() {
            $ionicHistory.nextViewOptions({
              disableBack: true
            });
            $state.go("app.home");
          })
          .catch(function(errorMsg) {
            NotificationService.showAlert({title: "error", template: errorMsg});
          })
          .finally(function() {
            $ionicLoading.hide();
          });
      })
      .catch(function(errorMsg) {
        NotificationService.showAlert({title: "error", template: errorMsg});
        $ionicLoading.hide();
      });
  };

  if (CashierModeService.isEnabled()) {
    $scope.openScanner();
  }

});
