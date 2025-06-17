from django.urls import path, include
from rest_framework import routers
from .views import *


urlpatterns = [
    path('', movie_list ,name='movie_list'), #Home page - lists all movies
    path('booking_history/', book_history, name='book_history'), #Gives the booking history of all users
    path('movies/<int:movie_pk>/book_seat', book_seat, name='book_seat'), # Allows a user to book a seat under their name
]
