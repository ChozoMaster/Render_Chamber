//var progress = setInterval(function () {
//    var $bar = $('.progress-bar') ;
//    var bar_value = $bar.attr('aria-valuenow') + '%';
//   // console.log(bar_value);
//    $bar.animate({ width: bar_value }, { duration: 2000, easing: 'easeInExpo' });
//}, 800);

$('.progress-bar').each(function () {
    $.easing.easeInBounce = function (x, t, b, c, d) {
        return c - $.easing.easeOutBounce (x, d-t, 0, c, d) + b;
    };

    $.easing.easeOutBounce = function (x, t, b, c, d) {
        if ((t/=d) < (1/2.75)) {
            return c*(7.5625*t*t) + b;
        } else if (t < (2/2.75)) {
            return c*(7.5625*(t-=(1.5/2.75))*t + .75) + b;
        } else if (t < (2.5/2.75)) {
            return c*(7.5625*(t-=(2.25/2.75))*t + .9375) + b;
        } else {
            return c*(7.5625*(t-=(2.625/2.75))*t + .984375) + b;
        }
    };

    var $bar = $(this) ;
    var bar_value = $bar.attr('aria-valuenow') + '%';
    $bar.animate({ width: bar_value }, 2000, 'easeInBounce', function() {
        $bar.text(bar_value);
     });
});

$("#special_href").click(function() {
    console.log(10);
    document.location.href='contact.html';
});

// Shorthand for $( document ).ready()
$(function() {
    var $width = $( window ).width();
    if($width < 1000)
    document.location.href='contact.html';
});

$( window ).resize(function() {
    var $width = $( window ).width();
    if($width < 1000)
    //    document.location.href='contact.html';
});
