from django.shortcuts import render,get_object_or_404, redirect
from rest_framework.renderers import TemplateHTMLRenderer
from rest_framework import status,permissions,viewsets, generics
from rest_framework.views import APIView
from rest_framework.response import Response
from rest_framework.decorators import action
from .models import *
from .serializers import *

#All viewsets allow our views to be modified using the api
class UserViewSet(viewsets.ModelViewSet):
    """
    API endpoint that allows seats to be viewed or edited.
    """
    #Queries the database for the objects needed for the view
    queryset = User.objects.all()
    serializer_class = UserSerializer #Then uses our serializer to create our crud operations

class SeatViewSet(viewsets.ModelViewSet):
    """
    API endpoint that allows seats to be viewed or edited.
    """
    queryset = Seat.objects.all()
    serializer_class = SeatSerializer

class MovieViewSet(viewsets.ModelViewSet):
    """
    API endpoint that allows movies to be viewed or edited.
    """
    queryset = Movie.objects.all()
    serializer_class = MovieSerializer

class BookingViewSet(viewsets.ModelViewSet):
    """
    API endpoint that allows bookings to be viewed or edited.
    """
    queryset = Booking.objects.all()
    serializer_class = BookingSerializer

#---------------------------------------------------------------------
#movie list - displays all movies in database
def movie_list(request):
        movies = Movie.objects.all() #database query
        return render(request, 'movie_list.html',{'movies': movies}) #renders query to html

#book history - displays all bookings in databse
def book_history(request):
        bookings = Booking.objects.all()
        return render(request, 'book_history.html',{'bookings': bookings})

#book seat - allows users to book a seat
def book_seat(request, movie_pk):
   #Get objects needed for booking
    movie = get_object_or_404(Movie, pk=movie_pk) #Gets pk based on the movie the user wanted to book for
    users = User.objects.all() #gets all user objects
    seats = Seat.objects.filter(is_booked=False) #gets all seats that are not booked

    # IF get...
    if request.method == 'GET':
        #Request grab all objects from above and redirect to page
        return render(request, 'book_seat.html', {'movie': movie, 'seats': seats, 'users':users}) # Render html passing movie

    #if post...
    if request.method == 'POST':
        #retreve the seat number and the user that was selected
        seat_number = request.POST.get('seat_number') # Grab seat selected
        seat = get_object_or_404(Seat, seat_number=seat_number) # Check seat is an object
        user_name = request.POST.get('user_name') #grab user
        user = get_object_or_404(User, name=user_name) # Check user is an object
    
        #create a booking object with that info
        Booking.objects.create(movie=movie, seat=seat, user=user)

        #set seat as booked & save it
        seat.is_booked = True
        seat.save() 

        return redirect('book_history')  # Redirect to booking history after booking
    









