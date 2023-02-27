from django.urls import path
from .views import (
	Home,
    show_static_html,
)

app_name = 'logic'

urlpatterns = [
	path('', Home.as_view(), name='home'),
    path('static_page/', show_static_html, name='static_page'),
]