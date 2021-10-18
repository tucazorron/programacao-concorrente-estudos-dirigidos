package models;

import java.util.Observable;
import java.util.Random;
import java.util.concurrent.Semaphore;

public class Car extends Observable implements Runnable{

    private Semaphore mutex;
    private Semaphore parkingPen;
    private Semaphore access;
    private Random random;

    private boolean[] parkingLots;
    private int usedLot;
    private int waitTime;

    public Car(Semaphore mutex, Semaphore parkingPen, Semaphore access, boolean[] parkingLots){
        this.mutex = mutex;
        this.parkingPen = parkingPen;
        this.parkingLots = parkingLots;
        this.access = access;
        random = new Random(System.currentTimeMillis());
    }

    @Override
    public void run() {

        try{
            //Trying to access to the parking lots
            parkingPen.acquire();
            mutex.acquire();
            if(ParkingPen.areLeaving){
                mutex.release();
                access.acquire();
            } else {
                mutex.release();
            }

            //END

            //Trying to find a free parking lot
            mutex.acquire();
            ParkingPen.areEntering = true;
            ParkingPen.areLeaving = false;
            boolean isOccupied = true;
            int i = 0;
            while (isOccupied){
                isOccupied = parkingLots[i];
                if(isOccupied){
                    i++;
                }
            }
            parkingLots[i] = true;
            usedLot = i;
            setChanged();
            notifyObservers("ENTER "+Thread.currentThread().getName() + " " + usedLot);
            waitTime = random.nextInt(4001) + 1000;
            ParkingPen.carsInside++;
            mutex.release();
            Thread.sleep(waitTime);
            access.release();
            //END

            //Attempting to leave the parking lot
            mutex.acquire();
            if(ParkingPen.areEntering){
                mutex.release();
                access.acquire();
            } else {
                mutex.release();
            }

            mutex.acquire();
            ParkingPen.areEntering = false;
            ParkingPen.areLeaving = true;
            ParkingPen.carsInside--;
            parkingLots[usedLot] = false;
            setChanged();
            notifyObservers("EXIT "+Thread.currentThread().getName() + " " + usedLot + " " + waitTime);
            parkingPen.release();
            access.release();
            mutex.release();
            //END

        }catch(InterruptedException e){
            e.printStackTrace();
        }
    }
}
