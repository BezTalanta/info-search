from django import template
from django.conf import settings

register = template.Library()

@register.simple_tag
def make_link_to_html(index):
    return settings.BASE_DIR / ("dirty_dump/" + str(index) + ".html")


@register.simple_tag(takes_context=True)
def check_ban(context, index):
    result = context['banned'].get(index, False)
    return result is not False, result
