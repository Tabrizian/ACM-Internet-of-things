angular.module('Admin', ['ngResource', 'ngRoute', 'ngTable'])
    .factory('Users', function($resource){
        return $resource('/users/:id');
    })
    .config(function($routeProvider){
        $routeProvider
            .when('/', {
                templateUrl: '/info',
                controller: 'DashboardController'
            })
            .when('/users', {
                templateUrl: '/admin/user_list',
                controller: 'UserListController'
            })
            .when('/user', {
                templateUrl: '/admin/user_form',
                controller: 'UserFormController'
            })
    });