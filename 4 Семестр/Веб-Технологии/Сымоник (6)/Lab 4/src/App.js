import React, { useState } from 'react';
import './App.css'

function Menu({
  numberOfAdults,
  numberOfChildren,
  numberOfInfants,
  classType,
  setNumberOfAdults,
  setNumberOfChildren,
  setNumberOfInfants,
  setClassType,
  setIsOpen
}) {


  return (
    <div className="menu">
      <div className="menu-item">

        <h3>Количество пассажиров</h3>
      </div>
      <div className="menu-item">
        <div className="counter">
          <button className="counter-button" onClick={() => setNumberOfAdults(numberOfAdults - 1)} disabled={numberOfAdults === 1}>-</button>
          <span className="counter-value">{numberOfAdults}</span>
          <button className="counter-button" onClick={() => setNumberOfAdults(numberOfAdults + 1)}>+</button>
        </div>
        <div>
          <div className="menu-label">Взрослые</div>
          <span className='light-span'>12 лет и старше</span>
        </div>
      </div>
      <div className="menu-item">
        <div className="counter">
          <button className="counter-button" onClick={() => setNumberOfChildren(numberOfChildren - 1)} disabled={numberOfChildren === 0}>-</button>
          <span className="counter-value">{numberOfChildren}</span>
          <button className="counter-button" onClick={() => setNumberOfChildren(numberOfChildren + 1)}>+</button>
        </div>
        <div>
          <div className="menu-label">Дети</div>
          <span className='light-span'>от 2 до 11 лет</span>
        </div>
      </div>
      <div className="menu-item">
        <div className="counter">
          <button className="counter-button" onClick={() => setNumberOfInfants(numberOfInfants - 1)} disabled={numberOfInfants === 0}>-</button>
          <span className="counter-value">{numberOfInfants}</span>
          <button className="counter-button" onClick={() => setNumberOfInfants(numberOfInfants + 1)}>+</button>
        </div>
        <div>
          <div className="menu-label">Младенцы</div>
          <span className='light-span'>Младше 2 лет, без места</span>
        </div>

      </div>
      <div className="menu-item">
        <div className="class-div">
          <h3>Класс</h3>
          <div className="radio-group">
            <div className="radio-item">
              <input type="radio" id="economy-radio" name="class-radio" value="economy" checked={classType === 'economy'} onChange={() => setClassType('economy')} />
              <label htmlFor="economy-radio">Эконом</label>
            </div>
            <div className="radio-item">
              <input type="radio" id="business-radio" name="class-radio" value="business" checked={classType === 'business'} onChange={() => setClassType('business')} />
              <label htmlFor="business-radio">Бизнес</label>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}

function Dropdown() {
  const [isOpen, setIsOpen] = useState(false);
  const [numberOfAdults, setNumberOfAdults] = useState(1);
  const [numberOfChildren, setNumberOfChildren] = useState(0);
  const [numberOfInfants, setNumberOfInfants] = useState(0);
  const [classType, setClassType] = useState('economy');

  return (
    <div className="dropdown">
      <button className="dropdown-button" onClick={() => setIsOpen(!isOpen)}>
        <span className="dropdown-label">{numberOfAdults + numberOfChildren + numberOfInfants} чел., {classType == 'economy' ? 'Эконом' : 'Бизнес'}</span>
        <span className={`dropdown-arrow ${isOpen ? 'open' : ''}`} />
      </button>
      {isOpen && (
        <Menu
          numberOfAdults={numberOfAdults}
          numberOfChildren={numberOfChildren}
          numberOfInfants={numberOfInfants}
          classType={classType}
          setNumberOfAdults={setNumberOfAdults}
          setNumberOfChildren={setNumberOfChildren}
          setNumberOfInfants={setNumberOfInfants}
          setClassType={setClassType}
          setIsOpen={setIsOpen}
        />
      )}
    </div>
  );
}



function FlightForm() {
  const [departureCity, setDepartureCity] = useState('');
  const [destinationCity, setDestinationCity] = useState('');
  const [departureDate, setDepartureDate] = useState('');
  const [returnDate, setReturnDate] = useState('');
  const [numberOfPassengers, setNumberOfPassengers] = useState(1);

  const handleSubmit = (event) => {
    event.preventDefault();
  }

  return (
    <div className='flight-form'>
      <div className='form-header'>
        <h1>Поиск дешёвых авиабилетов</h1>
        <strong>16 лет помогаем вам экономить</strong>
      </div>
      <form onSubmit={handleSubmit}>

        <input
          type="text"
          value={departureCity}
          onChange={(event) => setDepartureCity(event.target.value)}
          placeholder='Откуда'
        />

        <input
          type="text"
          value={destinationCity}
          onChange={(event) => setDestinationCity(event.target.value)}
          placeholder='Куда'

        />

        <input
          type="date"
          value={departureDate}
          onChange={(event) => setDepartureDate(event.target.value)}
        />

        <input
          type="date"
          value={returnDate}
          onChange={(event) => setReturnDate(event.target.value)}
        />

        <Dropdown></Dropdown>

        <button type="submit" className='submit'>Submit</button>
      </form>
    </div>
  );
}

function HotelForm() {
  const [city, setCity] = useState('');
  const [checkInDate, setCheckInDate] = useState('');
  const [checkOutDate, setCheckOutDate] = useState('');
  const [numberOfGuests, setNumberOfGuests] = useState(1);

  const handleSubmit = (event) => {
    event.preventDefault();
  }

  return (
    <div className='flight-form'>
      <div className='form-header'>
        <h1>Удобный поиск отелей</h1>
        <strong>Забронируйте идеальный номер по выгодной цене</strong>
      </div>
      <form onSubmit={handleSubmit}>
        <input
          type="text"
          value={city}
          onChange={(event) => setCity(event.target.value)}
          placeholder='Город или отель'
        />
        <input
          type="date"
          value={checkInDate}
          onChange={(event) => setCheckInDate(event.target.value)}
        />
        <input
          type="date"
          value={checkOutDate}
          onChange={(event) => setCheckOutDate(event.target.value)}
        />
        <input
          type="number"
          value={numberOfGuests}
          onChange={(event) => setNumberOfGuests(event.target.value)}
        />
        <button type="submit" className='submit'>Submit</button>
      </form>
    </div>

  );
}

function FormSwitcher() {
  const [currentForm, setCurrentForm] = useState('flight');

  return (
    <div className="App">
      <div className="button-container">
        <div class='button_forms'>
          <button className={currentForm === 'flight' ? 'active' : ''} onClick={() => setCurrentForm('flight')}>Авиабилеты</button>
          <button className={currentForm === 'hotel' ? 'active' : ''} onClick={() => setCurrentForm('hotel')}>Отели</button>
        </div>

      </div>
      {currentForm === 'flight' && <FlightForm />}
      {currentForm === 'hotel' && <HotelForm />}
    </div>
  );
}

export default FormSwitcher;