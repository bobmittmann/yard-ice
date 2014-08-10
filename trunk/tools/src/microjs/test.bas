IF number<0 THEN
  PRINT "Number is negative"
ELSEIF number>0 THEN
  PRINT "Number is positive"
ELSE
  PRINT "Number is zero"
END IF

for i = 0 to 126
for j = 0 to 126
for k = 0 to 10
let a = i * j * k
print a, i, j, k
next k
next j
next i
end

var a
