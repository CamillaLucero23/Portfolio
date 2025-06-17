from django.test import TestCase
from .models import *
import datetime
from django.urls import reverse
from rest_framework import status
from rest_framework.test import APITestCase

#Model tests...

class MovieTestCase(TestCase):
    def setUp(self):
        Movie.objects.create(title="Howl's Moving Castle", description="Studio Ghibli Film",release_date= datetime.date(2004,11,20),duration=129)
        
    def test_movie(self):
        howls = Movie.objects.get(title="Howl's Moving Castle")
        self.assertEqual(howls.title,"Howl's Moving Castle")

class SeatTestCase(TestCase):
    def setUp(self):
        Seat.objects.create(seat_number='2A', is_booked = False)
        
    def test_seat(self):
        book_seat = Seat.objects.get(seat_number='2A')
        book_seat.is_booked = True
        book_seat.save()
        self.assertEqual(book_seat.is_booked, True)

class BookingTestCase(TestCase):
    def setUp(self):
        Movie.objects.create(title="Howl's Moving Castle", description="Studio Ghibli Film",release_date= datetime.date(2004,11,20),duration=129)
        movie = Movie.objects.get(title="Howl's Moving Castle")
        Seat.objects.create(seat_number='2A', is_booked = False)
        seat = Seat.objects.get(seat_number="2A")
        User.objects.create(name="test")
        user = User.objects.get(name="test")
        Booking.objects.create(movie=movie, seat=seat, user=user)

    def test_booking(self):
        movie = Movie.objects.get(title="Howl's Moving Castle")
        booking = Booking.objects.get(movie=movie)
        self.assertEqual(booking.seat.seat_number, "2A") 

#-------------- API end points

class MovieAPITestCase(APITestCase):
    
    def setUp(self):
        # Create test movies
        Movie.objects.create(title="Movie 1", description="A test movie.", release_date="2023-01-01", duration=120)
        Movie.objects.create(title="Movie 2", description="Another test movie.", release_date="2023-02-01", duration=150)

    def test_get_movie_list(self):
        url = reverse('movie-list')  # Use the appropriate name from your urls.py
        response = self.client.get(url)

        # Assert that status code is 200
        self.assertEqual(response.status_code, status.HTTP_200_OK)

        # Assert that response data is a list
        self.assertIsInstance(response.data, list)

        # Assert the correct number of movies is returned
        self.assertEqual(len(response.data), 2)

        # Assert the structure and content of the first movie
        movie_data = response.data[0]
        self.assertIn('title', movie_data)
        self.assertIn('description', movie_data)
        self.assertIn('release_date', movie_data)
        self.assertEqual(movie_data['title'], "Movie 1")

class SeatAPITestCase(APITestCase):

    def setUp(self):
        # Create test seats
        Seat.objects.create(seat_number="A1", is_booked=False)
        Seat.objects.create(seat_number="A2", is_booked=True)

    def test_get_seat_list(self):
        url = reverse('seat-list')  # Replace with the correct URL name
        response = self.client.get(url)

        # Assert status code is 200 OK
        self.assertEqual(response.status_code, status.HTTP_200_OK)

        # Assert the response contains the correct data
        self.assertIsInstance(response.data, list)
        self.assertEqual(len(response.data), 2)

        # Check the first seat's details
        seat_data = response.data[0]
        self.assertEqual(seat_data['seat_number'], "A1")
        self.assertFalse(seat_data['is_booked'])


class BookingAPITestCase(APITestCase):

    def setUp(self):
        # Create a custom user with only a 'name' field
        self.user = User.objects.create(name='John Doe')

        # Create a movie
        self.movie = Movie.objects.create(title="Test Movie", description="A test movie", release_date="2023-01-01", duration=120)

        # Create some seats
        self.seat1 = Seat.objects.create(seat_number="A1", is_booked=True)
        self.seat2 = Seat.objects.create(seat_number="A2", is_booked=False)

        # Create bookings for the user
        self.booking1 = Booking.objects.create(movie=self.movie, seat=self.seat1, user=self.user)
        self.booking2 = Booking.objects.create(movie=self.movie, seat=self.seat2, user=self.user)

    def test_get_booking_list(self):
        url = reverse('booking-list')  # Replace with the correct URL name for booking list
        response = self.client.get(url)

        # Assert status code is 200 OK
        self.assertEqual(response.status_code, status.HTTP_200_OK)

        # Assert the response data is a list
        self.assertIsInstance(response.data, list)

        # Assert that two bookings are returned
        self.assertEqual(len(response.data), 2)

        # Assert the structure and content of the first booking
        booking_data = response.data[0]
        self.assertIn('movie', booking_data)
        self.assertIn('seat', booking_data)
        self.assertIn('user', booking_data)
