t_= NaN
x_= NaN
y_= NaN
z_= NaN
set palette negative
set palette defined (\
    1  '#FF0500', \
    2  '#000000', \
    3  '#000000', \
    4  '#545454', \
    5  '#6B6B6B', \
    6  '#929292', \
    7  '#A7A7A7', \
    8  '#C7C7C7', \
    9  '#D4D4D4', \
    10 '#E9E9E9', \
    11 '#FFFFFF', \
    12 '#DFFFDC' \
    )
splot 'HelloWorldSteps.dat' u 2:3:4:5:(dx= $2-x_,x_=$2,dy=$3-y_,y_=$3,dz=$4-z_,z_=$4,dt=$1-t_,t_=$1,sqrt(dx*dx+dy*dy+dz*dz)/(dt<0.01?0.01:dt)) title "Simulated spindle PWM with dependent coloring" with lines palette
