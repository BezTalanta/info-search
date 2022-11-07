from django.urls import path, re_path
from .views import (
	Home,
)

app_name = 'logic'

urlpatterns = [
	path('', Home.as_view(), name='home'),
]