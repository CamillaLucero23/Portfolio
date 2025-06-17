from django.db import models

#For Field Definitions: https://docs.djangoproject.com/en/5.1/ref/models/fields/

# Create your models here.

#User model - stores a name for a movie to be stored under when booked
class User(models.Model):
    name = models.CharField(max_length=20)

#Movie model - contains a movie's title, its description, its release date, and how long it is in minutes
class Movie(models.Model):
    title = models.CharField(max_length=200)
    description = models.TextField()
    release_date = models.DateField()
    duration = models.IntegerField()

#Seat model - contains the data for a seat number and whether it is booked or not
class Seat(models.Model):
    seat_number = models.CharField(max_length=3)
    is_booked = models.BooleanField(default=False)

#Booking model - Stores the data for a booking such as the movie object, the seat object, and the user object, aswell as 
# when the booking was made
class Booking(models.Model):
    movie = models.ForeignKey(Movie, on_delete=models.CASCADE)
    seat = models.ForeignKey(Seat, on_delete=models.CASCADE)
    user = models.ForeignKey(User, on_delete=models.CASCADE)
    booking_date = models.DateTimeField(auto_now_add=True)
