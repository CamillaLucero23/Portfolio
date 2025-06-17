"""
URL configuration for movie_theater_booking project.

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/4.2/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  path('', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  path('', Home.as_view(), name='home')
Including another URLconf
    1. Import the include() function: from django.urls import include, path
    2. Add a URL to urlpatterns:  path('blog/', include('blog.urls'))
"""
from django.contrib import admin
from django.urls import path, include
from rest_framework.routers import DefaultRouter
from bookings.views import MovieViewSet, SeatViewSet,BookingViewSet, UserViewSet

#This routes directly to api & registers our viewsets to connect the api to our models
router = DefaultRouter()
router.register(r'movies', MovieViewSet)
router.register(r'seats', SeatViewSet)
router.register(r'bookings', BookingViewSet)
router.register(r'users', UserViewSet)

#register our urls as the ones we have in bookings.urls, our router for our api, and admin for admin controls
urlpatterns = [
    path('', include('bookings.urls')),
    path('api/', include(router.urls)),
    path('admin/', admin.site.urls),
]
