from .models import *
from rest_framework import serializers
#All serializers - Used for the api!
class MovieSerializer(serializers.ModelSerializer):
    class Meta:
        model = Movie
        fields = ['title' ,'description' ,'release_date','duration']

class SeatSerializer(serializers.ModelSerializer):
    class Meta:
        model = Seat
        fields = ["seat_number","is_booked"]

class BookingSerializer(serializers.ModelSerializer):
    class Meta:
        model = Booking
        fields = ['movie','seat', 'user', 'booking_date']

class UserSerializer(serializers.ModelSerializer):
    class Meta:
        model = User
        fields = ['name']
