import App from './App';
import { connect } from 'react-redux';

const mapStateToProps = state => ({
  obiews: state.obiews,
  page: state.page,
})

export default connect(mapStateToProps)(App)
