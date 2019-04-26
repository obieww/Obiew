import Register from '../component/Register';
import { connect } from 'react-redux';

const mapStateToProps = state => ({
  obiews: state.obiews,
  page: state.page,
})

const mapDispatchToProps = dispatch => ({
  onCreateNewObiew: obiew => dispatch(createNewObiew(obiew)),
});

export default connect(mapStateToProps)(App)
