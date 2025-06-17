from django.apps import AppConfig

#This is what we need for our app to run!
class BookingsConfig(AppConfig):
    default_auto_field = 'django.db.models.BigAutoField'
    name = 'bookings'
