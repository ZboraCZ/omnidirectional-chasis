$uhel = @ARGV[0];

$Vx = cos($uhel * 0.0174532925);
$Vy = sin($uhel * 0.0174532925);

$Vx = sprintf("%.2f", $Vx);
$Vy = sprintf("%.2f", $Vy);

$m1Speed = -1 * $Vx;
$m2Speed = 0.5 * $Vx + sqrt(3)/2 * $Vy;
$m3Speed = 0.5 * $Vx - sqrt(3)/2 * $Vy;

$m1Speed = sprintf("%.2f", $m1Speed);
$m2Speed = sprintf("%.2f", $m2Speed);
$m3Speed = sprintf("%.2f", $m3Speed);

print "Uhel:$uhel° ; " , $uhel*0.0174532925, " rad\n";
print "Vx: $Vx; Vy: $Vy\n";
print "m1Speed: $m1Speed; m2Speed: $m2Speed; m3Speed: $m3Speed;\n";

