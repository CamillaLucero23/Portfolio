This project is a simple example of a movie-theater ticket booking site. 

The File structure is as follows:

manage.py -> how to server is started

Movie_theater_booking -
Contains any admin settings & intial server boot-up requirements. 

Bookings -
Contains models, views, and templates for site displayment. 
Contains tests.py for testing
Contains serializers for django restful api
Contains models migrations for django's use

----------------------------------------------------------------------
You can run this project locally by....
1. Setting up a python environment outside of this folder and activating it.

2.Once done, using that environment and pip to install all pip libraries within requirements.txt

3. Finally, going into movie_theater_booking, running "python manage.py runserver" on a port of your chosing, and 
    going into that host. 

------------------------------------------------------------------------------
You can run the unit and integration testing by....
1. entering movie_theater_booking, running "python manage.py test" which will cause django to run the tests automatically.