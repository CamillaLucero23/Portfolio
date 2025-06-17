from django.contrib import admin
from .models import *

# Register your models here.
#This registers your models in the django admin panel. 
admin.site.register(Booking)
admin.site.register(Seat)
admin.site.register(Movie)

